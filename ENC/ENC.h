/*
 * ENC.h
 *
 *  Created on: Dec 8, 2017
 *      Author: Fancy
 */

#ifndef ENC_ENC_H_
#define ENC_ENC_H_

uint8_t enc28j60ReadOp(uint8_t op, uint8_t address);
void enc28j60WriteOp(uint8_t op, uint8_t address, uint8_t data);
void enc28j60ReadBuffer(uint16_t len, uint8_t* rdata);
void enc28j60WriteBuffer(uint16_t len, uint8_t* wdata);
void enc28j60SetBank(uint8_t address);
uint8_t enc28j60Read(uint8_t address);
uint8_t enc28j60PhyReadH(uint8_t address);
void enc28j60Write(uint8_t address, uint8_t data);
void enc28j60PhyWrite(uint8_t address, uint16_t data);
void enc28j60clkout(uint8_t clk);
void enc28j60Init(uint8_t* macaddr);
void enc28j60GetMac(uint8_t *macddr);
uint8_t enc28j60getrev(void);
void enc28j60EnableBroadcast( void ) ;
void enc28j60DisableBroadcast( void );
uint8_t enc28j60linkup(void);
void enc28j60PacketSend(uint16_t len, uint8_t* packet);	/////
uint8_t enc28j60hasRxPkt(void);
uint16_t enc28j60PacketReceive(uint16_t maxlen, uint8_t* packet);

#endif /* ENC_ENC_H_ */
