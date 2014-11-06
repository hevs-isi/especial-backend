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

/* Debug print:  #define or #undef */
#define STM32_P103_EMUL_DEBUG

#ifdef STM32_P103_EMUL_DEBUG
#define DBG(fmt, ...) \
		printf("stm32p103_emul: " fmt, ## __VA_ARGS__);

#define PRINT_CMD(x) \
		printf(CMD_STR_PATTERN, x->data.periph, x->data.id, x->data.data)
#else
#define DBG(fmt, ...) \
		do { } while (0)

#define PRINT_CMD(x) \
		do { } while (0)
#endif

#define ERR(fmt, ...) \
		fprintf(stderr, "stm32p103_emul: " fmt, ## __VA_ARGS__)

static const char HOST_PATTERN[] = "localhost:%d";

static const uint32_t TCP_CMD_PORT = 14001;
static const char CMD_STR_PATTERN[] = "{\"periph\": %d, \"id\": %d, \"data\": %d}\n";

static const uint32_t TCP_EVT_PORT = 14002;


// Data structure for event in the queue
typedef struct P103Cmd {
	QSIMPLEQ_ENTRY(P103Cmd) entry; // element in the queue
	struct {
		P103PerId periph;
		uint32_t id;
		uint32_t data;
	} data;
} P103Cmd;

typedef struct P103EmulState {

	/* Send commands outside QEMU (to the monitor / GUI) */
	QSIMPLEQ_HEAD(cmd_list, P103Cmd) cmd_list; // queue of commands
	uint32_t cmd_nbr;
	uint32_t cmd_thread_terminate;
	QemuThread cmd_thread_id;
	QemuMutex cmd_mutex; // condition variable and associated mutex
	QemuCond cmd_cond;
	int cmd_sock; // TCP socket

	/* Receive events inside QEMU (from the monitor / GUI) */
    int evt_sock; // TCP socket
    QSIMPLEQ_HEAD(event_list, P103Cmd) evt_list; // queue of events
    QemuThread evt_thread_id;

} P103EmulState;

// State wit attributes
static P103EmulState p103_state;

/* Helper functions */
inline void post_event_digital_out(uint32_t id, uint32_t data) {
	stm32p103_emul_event_post(DIGITAL_OUT, id, data);
}

/**
 * Private helper method to count and add a command to the queue.
 */
void event_queue_add(P103Cmd *e);

void event_queue_add(P103Cmd *e) {
	// Insert at the end of the queue the new event
	qemu_mutex_lock(&p103_state.cmd_mutex);
	QSIMPLEQ_INSERT_TAIL(&(p103_state.cmd_list), e, entry);
	p103_state.cmd_nbr += 1;
	qemu_cond_signal(&p103_state.cmd_cond);
	qemu_mutex_unlock(&p103_state.cmd_mutex);
}

/** Public functions */

void* stm32p103_emul_event_post(P103PerId periph, uint32_t id, uint32_t data) {
	P103Cmd *cmd;

	// Create the event
	cmd = g_malloc(sizeof(P103Cmd));
	cmd->data.periph = periph;
	cmd->data.id = id;
	cmd->data.data = data;
	event_queue_add(cmd); // Add it in the queue

	DBG("post command, size %02d: ", p103_state.cmd_nbr);
	PRINT_CMD(cmd);

	if (p103_state.cmd_nbr > 16)
		ERR("WARN: %d commands in the queue\n", p103_state.cmd_nbr);

	return NULL;
}

/**
 * Thread to sent commands outside QEMU through a TCP socket.
 */
static void *stm32p103_emul_cmd_thread(void *arg) {
	P103EmulState *state = arg; // program state
	P103Cmd *cmd;

	DBG("%s started\n", __FUNCTION__);

	char host_str[255];
	snprintf(host_str, sizeof(host_str), HOST_PATTERN, TCP_CMD_PORT);
	state->cmd_sock = inet_connect(host_str, NULL);

	if (state->cmd_sock == -1) {
		ERR("failed to connect to %s\n", host_str);
		ERR("%s terminated\n\n", __FUNCTION__);
		return NULL;
	}

	DBG("connected to %s\n\n", host_str);

	while (1) {
		/* Wait on command to process */
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

				char cmd_str[512];
				size_t count = snprintf(cmd_str, sizeof(cmd_str),
						CMD_STR_PATTERN, cmd->data.periph, cmd->data.id,
						cmd->data.data);
				int res = write(state->cmd_sock, cmd_str, count);
				if (res == -1) {
					ERR("failed to send\n");
					ERR("%s terminated\n", __FUNCTION__);
					qemu_mutex_unlock(&state->cmd_mutex);
					g_free(cmd);
					return NULL;
				}

				// Command processed successfully
				else {
					QSIMPLEQ_REMOVE_HEAD(&state->cmd_list, entry);
					state->cmd_nbr -= 1;

					DBG("send command, rest %02d: ", state->cmd_nbr);
					PRINT_CMD(cmd);
				}
			}
			qemu_mutex_unlock(&state->cmd_mutex);
			g_free(cmd);
		}
	}

	DBG("%s terminated\n", __FUNCTION__);
	return NULL;
}

/**
 * Read events from a Monitor to write QEMU inputs.
 */
static void *stm32p103_emul_evt_handle(void *arg) {
	P103EmulState *state = arg; // program state

	DBG("%s started\n", __FUNCTION__);

	char host_str[255];
	snprintf(host_str, sizeof(host_str), HOST_PATTERN, TCP_EVT_PORT);
	state->evt_sock = inet_connect(host_str, NULL);

	if (state->evt_sock == -1) {
		ERR("failed to connect to %s\n", host_str);
		ERR("%s terminated\n\n", __FUNCTION__);
		return NULL;
	}

	DBG("connected to %s\n\n", host_str);

	uint32_t data;
	while(1) {

		// Read from server
		DBG("Wait to read...\n");
		size_t n = read(state->evt_sock, &data, 4);
		data = ntohl(data);
		if(n != 4) {
			DBG("Read %d. End !\n", (int)n);
			DBG("Read: %d\n\n", data);
			break; // Connection closed or error
		}
		DBG("Read OK: %d\n", data);

		/*read(sp6->evt_sock, &per_id, 4);
		per_id = ntohl(per_id);
		read(sp6->evt_sock, &action, 4);
		action = ntohl(action);
		read(sp6->evt_sock, &len, 4);
		len = ntohl(len);
		DBG("%s p: 0x%08X, a: 0x%08X, l: 0x%08X\n", __FUNCTION__, per_id, action, len);

		buf = malloc(len);
		read(sp6->evt_sock, buf, len);

		switch (per_id) {
			case SP6_BTN:
				reptar_sp6_btns_event_process(action, buf, len);
				free(buf);
				break;
			default:
				DBG("%s invalid peripheral id\n", __FUNCTION__);
		}*/
	}

	DBG("%s terminated\n", __FUNCTION__);
	return NULL;
}

int stm32p103_emul_init(void) {
	DBG("%s\n", __FUNCTION__);

	// State initialization
	QSIMPLEQ_INIT(&p103_state.cmd_list);
	p103_state.cmd_nbr = 0;	// empty queue
	p103_state.cmd_thread_terminate = 0;
	qemu_mutex_init(&p103_state.cmd_mutex);
	qemu_cond_init(&p103_state.cmd_cond);

	// Thread to send commands outside QEMU
	qemu_thread_create(&p103_state.cmd_thread_id, "cmd", stm32p103_emul_cmd_thread, &p103_state, QEMU_THREAD_JOINABLE);

	// Thread to receive events from the Monitor / GUI
	qemu_thread_create(&p103_state.evt_thread_id, "evt", stm32p103_emul_evt_handle, &p103_state, QEMU_THREAD_JOINABLE);

	return 0;
}

int stm32p103_emul_exit(void) {
	DBG("%s\n", __FUNCTION__);

	p103_state.cmd_thread_terminate = 1;
	qemu_cond_signal(&p103_state.cmd_cond);
	qemu_thread_join(&p103_state.cmd_thread_id);

	qemu_thread_join(&p103_state.evt_thread_id);

	return 0;
}
