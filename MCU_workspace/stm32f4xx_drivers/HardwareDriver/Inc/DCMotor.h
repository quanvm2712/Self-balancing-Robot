/*
 * DCMotor.h
 *
 *  Created on: Jun 1, 2025
 *      Author: quanvm198
 */

#ifndef INC_DCMOTOR_H_
#define INC_DCMOTOR_H_


#include "stm32f407xx.h"


/*
 * L298N GPIO Pin
 */
#define	L298N_IN1_PORT		GPIOC
#define	L298N_IN1_PIN		GPIO_PIN_2

#define	L298N_IN2_PORT		GPIOC
#define	L298N_IN2_PIN		GPIO_PIN_3


/*
 * Define motor name
 */
#define	MOTOR_LEFT		0
#define MOTOR_RIGHT		1


/*
 * Motor direction definition
 */
#define	MOTOR_DIR_FORWARD	0
#define	MOTOR_DIR_BACKWARD	1
#define MOTOR_DIR_STOP		2


/*/
 * User function
 */
void Motor_ConfigIN_GPIO(void);
void Motor_ConfigDirection(_Bool Motor, _Bool Direction);
void Motor_ConfigPWMSource(void);
void Motor_Init(void);
void Motor_Control(_Bool Motor, int16_t ControlSignal);

#endif /* INC_DCMOTOR_H_ */
