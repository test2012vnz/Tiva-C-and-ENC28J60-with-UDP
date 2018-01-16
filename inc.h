

#ifndef INC_H_
#define INC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"

#include "Delay/Delay.h"
#include "UART/uartstdio.h"
#include "UART/UART.h"
#include "UART/debug.h"
#include "SPI/SPI.h"
#include "ENC/ENC.h"
#include "ENC/ENCREG.h"
#include "Ethernet/Ethernet.h"
#include "Ethernet/ARP.h"
#include "Ethernet/IP.h"
#include "Ethernet/UDP.h"
#include "Ethernet/DHCP.h"
#include "Ethernet/ICMP.h"
#include "Ethernet/TCP.h"

#define HTONS(s)		(uint16_t)((s<<8) | (s>>8))	//danh cho bien 2 byte
#define HTONL(l)		(uint32_t)((l<<24) | ((l&0x00FF0000l)>>8) | ((l&0x0000FF00l)<<8) | (l>>24))	//danh cho bien 4 byte
uint8_t eth_got_frame;

struct udpHeader *udpPkTest;
struct ipHeader *ipPKTest;
struct arpHeader *arpPKTest;
struct tcpHeader *tcpPKtest;

#endif /* INC_H_ */
