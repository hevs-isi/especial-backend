/**
 * \file pid.h
 * \brief implementation of a PID using double values
 *
 * \author marc dot pignat at hevs dot ch
 */

#pragma once

#include <stdfix.h>
#include "utils/time.h"

#ifdef __cplusplus
extern "C" {
#endif

struct pid_state_t {
	double integral;
	double previous_error;
	double setpoint;
	double measure;
	double output;
	timeout_t time;
};

typedef struct {
	struct pid_state_t state;
	double kp;
	double ki;
	double kd;

	double min;
	double max;
} pid_t;

void pid_init(pid_t *pid, double kp, double ki, double kd, double min, double max);
double pid_step(pid_t *pid, double measured_value);

#ifdef __cplusplus
}
#endif
