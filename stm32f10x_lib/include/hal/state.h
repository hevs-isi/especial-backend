#ifndef STATE_H
#define STATE_H

/**
 * @brief The state of a digital input or output.
 */
typedef enum State_t {
	Off = -1, /*!< GPIO is OFF */
	On = 1, /*!< GPIO is ON */
	Invalid = 0 /*!< Invalid state (not initialized) */
} State;

#endif
