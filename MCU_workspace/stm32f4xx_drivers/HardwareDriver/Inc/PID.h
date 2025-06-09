/*
 * PID.h
 *
 *  Created on: Jun 8, 2025
 *      Author: nhduo
 */

#ifndef INC_PID_H_
#define INC_PID_H_

typedef struct
{
	double Kp;
	double Ki;
	double Kd;
	double integral;
	double prev_error;
}PID_Controller;


void PID_Init(PID_Controller *pid, double Kp, double Ki, double Kd);
double PID_Compute(PID_Controller *pid, double setpoint, double measured);
#endif /* INC_PID_H_ */
