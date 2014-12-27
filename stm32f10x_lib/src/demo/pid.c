/**
 * \file pid.c
 * \see pid.h
 */
#include "demo/pid.h"

void pid_init(pid_t *pid, double kp, double ki, double kd, double min, double max) {
	pid->state.time = time_get();
	pid->state.integral = 0;
	pid->state.previous_error = 0;
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->min = min;
	pid->max = max;
}

double pid_step(pid_t *pid, double measured_value) {
	// Saturation added for the integral part.
	// Return the output value.

	timeout_t current_time = time_get();
	const double dt = 0.001 * time_diff_ms(current_time, pid->state.time);
	pid->state.time = current_time;
	const double error = pid->state.setpoint - measured_value;
	const double derivative = (error - pid->state.previous_error) / dt;

	pid->state.measure = measured_value;

	// Min and max saturation for the integral part
	pid->state.integral += error * dt;
	if (pid->state.integral > pid->max)
		pid->state.integral = pid->max;
	if (pid->state.integral < pid->min)
		pid->state.integral = pid->min;

	pid->state.previous_error = error;
	pid->state.output = pid->kp * error + pid->ki * pid->state.integral
			+ pid->kd * derivative;

	// Min and max saturation for the output
	if (pid->state.output > pid->max)
		pid->state.output = pid->max;
	if (pid->state.output < pid->min)
		pid->state.output = pid->min;

	return pid->state.output; // The output value
}
