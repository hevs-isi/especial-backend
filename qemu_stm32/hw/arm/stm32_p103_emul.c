/*
 * REPTAR Spartan6 FPGA emulation
 * Emulation "logic" part. Gateway between the emulation code and the backend.
 *
 * Copyright (c) 2013 HEIG-VD / REDS
 * Written by Romain Bornet
 *
 * This code is licensed under the GPL.
 */

#include "qemu-common.h"
#include "qemu/sockets.h"
#include "qemu/thread.h"
#include "qemu/queue.h"

#include "hw/arm/stm32_p103_emul.h"
#include "hw/arm/stm32.h"

#ifdef STM32_P103_DEBUG
#define PRINT_CMD_PERIPH(x) \
		printf(CMD_PERIPH, x->data.id, x->data.port, x->data.pin, x->data.value)
#define PRINT_CMD_EVENT(x) \
		printf(CMD_EVENT, x->data.id, x->data.value)
#else
#define PRINT_CMD_PERIPH(x) \
		do { } while (0)
#define PRINT_CMD_EVENT(x) \
		do { } while (0)
#endif

static const char HOST_PATTERN[] = "localhost:%d";

static const uint32_t TCP_CMD_PORT = 14001;
static const char CMD_PERIPH[] = "{\"msgId\":%d,\"pin\":{\"port\":\"%c\",\"nbr\":%d},\"value\":%d}\n";
static const char CMD_EVENT[] = "{\"msgId\":%d,\"value\":%d}\n";

static const uint32_t TCP_EVT_PORT = 14002;


// Data structure for event in the queue
typedef struct P103Cmd {
	QSIMPLEQ_ENTRY(P103Cmd) entry; // element in the queue
	struct {
		EventId id;		//<! Action id
		uint8_t port;		//<! GPIO port (as a letter)
		uint8_t pin;		//<! GPIO pin number (from 0 to 15)
		uint32_t value;		//<! Event value
	} data;
} P103Cmd;

typedef struct P103EmulState {

	/* Send commands outside QEMU (to the monitor / GUI) */
	QSIMPLEQ_HEAD(cmd_list, P103Cmd) cmd_list; // queue of commands
	uint32_t cmd_list_size; // size of the command list
	uint32_t cmd_thread_terminate;
	QemuThread cmd_thread_id;
	QemuMutex cmd_mutex; // condition variable and associated mutex
	QemuCond cmd_cond;
	int cmd_sock; // TCP socket

	/* Receive events inside QEMU (from the monitor - Scala side) */
    int evt_sock; // TCP socket
    QSIMPLEQ_HEAD(event_list, P103Cmd) evt_list; // queue of events
    QemuThread evt_thread_id;
    QemuMutex evt_mutex;
    QemuCond evt_ack_cond; // signal when the event has been confirmed

} P103EmulState;

static P103EmulState p103_state; // State wit attributes
static Stm32P103* stm_state; // STM32p103 STK state

/* Private function */

/**
 * Add an event to the queue and send it through TCP.
 * \param[in] id	the id to identify the type of the event
 * \param[in] port	the port letter of the GPIO (if used)
 * \param[in] pin	the pin number of the GPIO (if used)
 * \param[in] value	the event value
 */
void stm32p103_emul_event_post(EventId id, uint8_t port, uint8_t pin, uint32_t value);


/* Helper functions */

inline void post_event_digital_out(uint8_t port, uint8_t pin, uint32_t value) {
	stm32p103_emul_event_post(DIGITAL_OUT, port, pin, value);
}

inline void post_event_c(uint8_t eventId) {
	// Pin and port are not used
	stm32p103_emul_event_post(C_EVENT, 0, 0, (uint32_t)(eventId));
}


// The USART5 wait until the QEMU event has bee acked by the Scala code
void event_wait_ack(void) {
	qemu_mutex_lock(&p103_state.evt_mutex);
	qemu_cond_wait(&p103_state.evt_ack_cond, &p103_state.evt_mutex);
	qemu_mutex_unlock(&p103_state.evt_mutex);
}

/**
 * Private helper method to count and add a command to the queue.
 */
void event_queue_add(P103Cmd *e);

void event_queue_add(P103Cmd *e) {
	// Insert at the end of the queue the new event
	qemu_mutex_lock(&p103_state.cmd_mutex);
	QSIMPLEQ_INSERT_TAIL(&(p103_state.cmd_list), e, entry);
	p103_state.cmd_list_size += 1;
	qemu_cond_signal(&p103_state.cmd_cond);
	qemu_mutex_unlock(&p103_state.cmd_mutex);
}

/** Public functions */

void stm32p103_emul_event_post(EventId id, uint8_t port, uint8_t pin, uint32_t value) {
	P103Cmd *cmd;

	const uint32_t size = p103_state.cmd_list_size;

	// Create the event
	cmd = g_malloc(sizeof(P103Cmd));
	cmd->data.id = id;
	cmd->data.port = port; // Optional if it is an event
	cmd->data.pin = pin;   // Optional if it is an event
	cmd->data.value = value;
	event_queue_add(cmd); // Add it in the queue

	DBG("_emul: cmd queued: ");
	if(id == C_EVENT)
		PRINT_CMD_EVENT(cmd);
	else
		PRINT_CMD_PERIPH(cmd);

	// Exit the program if too many messages are stored
	if (size > 1023) {
		ERR("_emul: %04d commands in the queue.\n", size);
		ERR("_emul: Program exit !\n\n");
		exit(-1);	// Return an error code
	}
	else if (size > 15)
		DBG("_emul: %04d commands in the queue\n", size);
}

/**
 * Thread used to send commands outside QEMU through a TCP socket.
 */
static void *stm32p103_emul_cmd_thread(void *arg) {
	P103EmulState *state = arg; // program state
	P103Cmd *cmd;

	char host_str[32];

	char cmd_str[128];
	size_t count;

	DBG("_emul: %s started\n", __FUNCTION__);

	snprintf(host_str, sizeof(host_str), HOST_PATTERN, TCP_CMD_PORT);
	state->cmd_sock = inet_connect(host_str, NULL);

	if (state->cmd_sock == -1) {
		ERR("_emul: failed to connect to %s\n", host_str);
		ERR("_emul: %s terminated\n\n", __FUNCTION__);
		return NULL;
	}

	DBG("_emul: connected to %s\n\n", host_str);

	while (1) {
		// Wait on command to process
		qemu_mutex_lock(&state->cmd_mutex);
		qemu_cond_wait(&state->cmd_cond, &state->cmd_mutex);
		qemu_mutex_unlock(&state->cmd_mutex);

		if (state->cmd_thread_terminate)
			break; // Exit thread

		while (!QSIMPLEQ_EMPTY(&state->cmd_list)) {
			qemu_mutex_lock(&state->cmd_mutex);
			{
				// Pop the first command from the queue
				cmd = QSIMPLEQ_FIRST(&state->cmd_list);

				// Format messages to JSON frames
				if(cmd->data.id == C_EVENT) {
					count = snprintf(cmd_str, sizeof(cmd_str),
							CMD_EVENT, cmd->data.id, cmd->data.value);
				}
				else if(cmd->data.id == DIGITAL_OUT) {
					count = snprintf(cmd_str, sizeof(cmd_str),
							CMD_PERIPH, cmd->data.id, cmd->data.port,
							cmd->data.pin, cmd->data.value);
				}

				// Send the JSON message
				int res = write(state->cmd_sock, cmd_str, count);
				if (res == -1) {
					ERR("_emul: failed to send\n");
					ERR("_emul: %s terminated\n", __FUNCTION__);
					qemu_mutex_unlock(&state->cmd_mutex);
					g_free(cmd);
					return NULL;
				}

				// Command processed successfully
				else {
					QSIMPLEQ_REMOVE_HEAD(&state->cmd_list, entry);
					state->cmd_list_size -= 1;
				}
			}
			qemu_mutex_unlock(&state->cmd_mutex);
			g_free(cmd);
		}
	}
	return NULL;
}

/**
 * Read events from a Monitor to write QEMU inputs.
 */
static void *stm32p103_emul_evt_handle(void *arg) {
	P103EmulState *state = arg; // program state

	DBG("_emul: %s started\n", __FUNCTION__);

	char host_str[255];
	snprintf(host_str, sizeof(host_str), HOST_PATTERN, TCP_EVT_PORT);
	state->evt_sock = inet_connect(host_str, NULL);

	if (state->evt_sock == -1) {
		ERR("_emul: failed to connect to %s\n", host_str);
		ERR("_emul: %s terminated\n\n", __FUNCTION__);
		return NULL;
	}

	DBG("_emul: connected to %s\n\n", host_str);

	// Data read from the monitor server
	typedef union
	{
	    uint32_t nbr;
	    uint8_t byte[4];
	} Value;
	Value data;

	while(1) {

		// Read from server
		size_t n = read(state->evt_sock, &data, 4);
		data.nbr = ntohl(data.nbr);

		if(n == 0) {
			break;
		}
		else if(n != 4) {
			DBG("_emul: Read %d. End !\n", (int)n);
		}

		// 4 bytes received from the Scala side
		else {

			const uint8_t id = data.byte[3];

			// Set digital input pin value
			if(id == SCALA_DIGITAL_IN) {
				const uint8_t btId = data.byte[1];
				qemu_irq btIrq = NULL;

				// Only the 4 input buttons are supported
				switch(btId) {
				case 0:
					btIrq = stm_state->btn0_irq;
					break;
				case 1:
					btIrq = stm_state->btn1_irq;
					break;
				case 2:
					btIrq = stm_state->btn2_irq;
					break;
				case 3:
					btIrq = stm_state->btn3_irq;
					break;
				default:
					// ERR
					break;
				}

				// Update the input value if the pin number is valid
				if(btIrq != NULL) {
					const uint8_t btValue = data.byte[0];
					if(btValue == 0) {
						// qemu_irq_raise(btIrq);
						DBG("_emul: LOWER *********\n")
						//qemu_set_irq(btIrq, 0);
						//qemu_irq_lower(btIrq);

						//qemu_set_irq(stm_state->btn2_irq, 1);
						//qemu_set_irq(stm_state->btn2_irq, 0);
					}
					else {
						// qemu_irq_lower(btIrq);
						DBG("_emul: RAISE *********\n")
						//qemu_set_irq(btIrq, 1);
						//qemu_irq_raise(btIrq);

						//qemu_set_irq(stm_state->btn2_irq, 0);
						//qemu_set_irq(stm_state->btn2_irq, 1);
					}

					DBG("_emul: BT [3]=%d, [2]=%d, [1]=%d, [0]=%d\n", data.byte[3], data.byte[2], data.byte[1], data.byte[0]);

					DBG("_emul: Button number %d set to '%d'\n", btId, btValue)
				}
			}

			// QEMU logger event ack from the Scala side. Continue the execution of the QEMU code.
			else if(id == SCALA_ACK_EVT) {
				// Event confirmed from the Monitor server. Run the code until the next event.

				/*qemu_set_irq(stm_state->btn2_irq, toggle);
				if(toggle == 0) {
					DBG("UP ------------")
					toggle = 1;
				}
				else {
					DBG("DOWN ------------")
					toggle = 0;
				}*/

				DBG("_emul: ACK event received.\n");
				qemu_mutex_lock(&p103_state.evt_mutex);
				qemu_cond_signal(&p103_state.evt_ack_cond);
				qemu_mutex_unlock(&p103_state.evt_mutex);
			}
		}
	}
	return NULL;
}

int stm32p103_emul_init(Stm32P103* s) {
	DBG("_emul: %s\n", __FUNCTION__);

	// State initialization
	stm_state = s;

	// TCP write
	QSIMPLEQ_INIT(&p103_state.cmd_list);
	p103_state.cmd_list_size = 0;	// empty queue
	p103_state.cmd_thread_terminate = 0;
	qemu_mutex_init(&p103_state.cmd_mutex);
	qemu_cond_init(&p103_state.cmd_cond);

	// TCP read
	qemu_mutex_init(&p103_state.evt_mutex);
	qemu_cond_init(&p103_state.evt_ack_cond);

	// Thread to send commands outside QEMU
	qemu_thread_create(&p103_state.cmd_thread_id, "cmd", stm32p103_emul_cmd_thread, &p103_state, QEMU_THREAD_JOINABLE);

	// Thread to receive events from the Monitor / GUI
	qemu_thread_create(&p103_state.evt_thread_id, "evt", stm32p103_emul_evt_handle, &p103_state, QEMU_THREAD_JOINABLE);

	return 0;
}

int stm32p103_emul_exit(void) {
	DBG("_emul: %s\n", __FUNCTION__);

	p103_state.cmd_thread_terminate = 1;
	qemu_cond_signal(&p103_state.cmd_cond);
	qemu_thread_join(&p103_state.cmd_thread_id);

	qemu_cond_signal(&p103_state.evt_ack_cond);
	qemu_thread_join(&p103_state.evt_thread_id);

	return 0;
}
