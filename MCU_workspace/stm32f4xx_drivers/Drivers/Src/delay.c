/*
 * delay.c
 *
 *  Created on: Feb 23, 2025
 *      Author: nhduo
 */

#ifndef SRC_DELAY_C_
#define SRC_DELAY_C_

#include "delay.h"

void delay_ms(uint32_t ms)
{
    volatile uint32_t count;
    for (uint32_t i = 0; i < ms; i++) {
        count = 16000;  // Approximate loop count for 1ms (based on 16 MHz clock)
        while (count--) {
            // Busy-wait loop
        }
    }
}
#endif /* SRC_DELAY_C_ */
