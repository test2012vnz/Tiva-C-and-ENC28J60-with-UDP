/*
 * UART.h
 *
 *  Created on: Jan 6, 2018
 *      Author: Fancy
 */

#ifndef UART_UART_H_
#define UART_UART_H_

char UART0_buf[100];
int bufCount_0;

void UART0_Config(void);
void Reset(char *pbuff);
#endif /* UART_UART_H_ */
