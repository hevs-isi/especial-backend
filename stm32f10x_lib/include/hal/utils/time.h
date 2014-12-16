/************************************************************************//**
 * \file	heivs/time.h
 * \brief	simple time abstraction
 * \author	marc dot pignat at hevs dot ch
 *
 * Provides a simple way to manage time and timeouts.
 * Code adapted for the STM32F103-STK board.
 ***************************************************************************/
#ifndef HEIVS_TIME_H
#define HEIVS_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief Timeout structure
 *
 * This structure is used to hold a timeout value. This structure only holds a
 * single integer, this trick is used to prevent erroneous operations that can
 * be done on integers, but not on this value.
 *
 * This protection comes at no CPU or memory cost.
 */
typedef struct
{
	uint32_t value;
} timeout_t;

/**
 * \brief Get current time in obscure format from boot
 *
 * \return the current time
 *
 * \warning will overlap
 */
static inline timeout_t time_get(void)
{
	/**
	 * There is no need for locking, there is only one cpu in the system
	 * and it will update the gTimeCounter in an atomic manner.
	 */
	extern volatile uint32_t gTimeCounter;
	/*timeout_t time =
	{
			.value = gTimeCounter,
	};*/

	timeout_t time;
	time.value = gTimeCounter;

	return time;
}

static inline uint32_t time_to_ms(timeout_t t)
{
	return t.value;
}

/**
 * \brief Is this time passed?
 * \param timeout the timeout value
 * \return zero if this time has not elapsed
 */
static inline uint32_t time_elapsed(timeout_t timeout)
{
	timeout_t now = time_get();
	return (timeout.value - now.value) > INT32_MAX;
}

/**
 * \brief Set an obscure time at least ms milliseconds in the future
 *
 * \param ms the minimum delay
 * \return the timeout obscure variable
 *
 * \see time_elapsed
 *
 * time_elapsed will return true when *more* than ms have passed
 */
timeout_t time_set_timeout_ms(uint32_t ms);

uint32_t time_diff_ms(timeout_t now, timeout_t before);

/**
 * \brief Timer initialization. Use the timer 2.
 */
void time_init(void);

void time_suspend(void);

void time_resume(void);

#ifdef __cplusplus
}
#endif

#endif /* HEIVS_TIME_H */
