/*
 * ICMP.h
 *
 *  Created on: Jan 12, 2018
 *      Author: Fancy
 */

#ifndef ETHERNET_ICMP_H_
#define ETHERNET_ICMP_H_

#define ICMP_HEADER_LEN			8
#define ICMP_TYPE_ECHOREPLY		0
#define ICMP_TYPE_ECHOREQUEST	8

struct icmpHeader
{
	uint8_t		Type;
	uint8_t		Code;
	uint16_t	Checksum;
	uint16_t	ID;
	uint16_t	seqNumber;
};

void icmpEchoReply(struct ipHeader* packet);
void icmpRequest(uint32_t dstIP, struct ipHeader* packet);
void icmpIpIn(struct ipHeader* packet);

uint8_t *pingAddr;
#endif /* ETHERNET_ICMP_H_ */
