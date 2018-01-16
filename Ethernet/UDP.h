/*
 * UDP.h
 *
 *  Created on: Jan 12, 2018
 *      Author: Fancy
 */

#ifndef ETHERNET_UDP_H_
#define ETHERNET_UDP_H_

#define UDP_HEADER_LEN	8
#define MAXPACKETLEN 600
struct udpHeader
{
	uint16_t	srcPort;
	uint16_t	desPort;
	uint16_t	Len;
	uint16_t	Checksum;
	uint8_t 	data[1];
};
struct udpReplyPK{
	uint16_t	srcPort;
	uint16_t	desPort;
	uint16_t	Len;
	uint16_t	Checksum;
	uint8_t data[MAXPACKETLEN];
};
void udpSend(uint32_t dstIp, uint16_t dstPort, uint16_t srcPort, uint16_t len, uint8_t* udpData);
void UDPProcess(uint16_t len, struct ipHeader* packet);

void UDPReply(uint32_t dstIP, uint16_t dstPort,uint8_t *str);
void UDPParse_msg(struct ipHeader* packet);

uint8_t udpBuffer[MAXPACKETLEN];
#endif /* ETHERNET_UDP_H_ */
