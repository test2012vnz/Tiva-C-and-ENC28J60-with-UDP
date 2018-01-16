/*
 * Delay.h
 *
 *  Created on: Apr 24, 2017
 *      Author: Fancy
 */

#ifndef DELAY_DELAY_H_
#define DELAY_DELAY_H_

void InitTime(void);
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);
uint32_t millis(void);
uint32_t micros(void);
void Timer_Config(void);

#endif /* DELAY_DELAY_H_ */
