/*
 * mc.h
 *
 *  Created on: May 4, 2026
 *      Author: avais
 */

#ifndef MC_H_
#define MC_H_

#define PWM_MAX  999
#define PWM_REVERSE 500
#define PWM_INNER_TURN 250
#define PWM_OUTER_TURN 600


void motors_stop(void);
void motors_forward(void);
void motors_backward(void);
void motors_left(void);
void motors_right(void);
void PWM_init(void);
void timer_init(void);


#endif /* MC_H_ */
