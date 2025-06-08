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
  float Kp;
  float Ki;
  float Kd;
  float integral;
  float prev_error;
}PID_Controller;


void PID_Init(PID_Controller *pid, float Kp, Ki, Kd);
float PID_Compute(PID_Controller *pid, float setpoint, folat measured, float dt);
#endif /* INC_PID_H_ */
