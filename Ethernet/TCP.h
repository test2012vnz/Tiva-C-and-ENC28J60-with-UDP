/*
 * TCP.h
 *
 *  Created on: Jan 13, 2018
 *      Author: Fancy
 */

#ifndef ETHERNET_TCP_H_
#define ETHERNET_TCP_H_

#define MAX_SEGMENT_SIZE	(ETHERNET_BUFFER_SIZE - ETH_HEADER_LEN - IP_HEADER_LEN - TCP_HEADER_LEN)
#define TCP_TIMEOUT 		60
#define TCP_MAX_SESSION 	8

// Flags
#define TCP_NON_FLAG (0)
#define TCP_FIN_FLAG (1<<0)
#define TCP_SYN_FLAG (1<<1)
#define TCP_RST_FLAG (1<<2)
#define TCP_PSH_FLAG (1<<3)
#define TCP_ACK_FLAG (1<<4)
#define TCP_URG_FLAG (1<<5)
#define TCP_ECE_FLAG (1<<6)
#define TCP_CWR_FLAG (1<<7)
//TCP session state
#define TCP_STATE_CLOSED       0
#define TCP_STATE_SYN_SENT     1
#define TCP_STATE_LISTEN       2
#define TCP_STATE_SYN_RECEIVED 3
#define TCP_STATE_ESTABLISHED  4
#define TCP_STATE_FIN_WAIT1    5
#define TCP_STATE_FIN_WAIT2    6
#define TCP_STATE_CLOSING      7
#define TCP_STATE_TIMED_WAIT   8
#define TCP_STATE_CLOSE_WAIT   9
#define TCP_STATE_LAST_ACK    10

struct tcpHeader
{
	uint16_t Srcport;
	uint16_t DstPort;
	uint8_t SeqNumber[4];
	uint8_t AckNumber[4];
	uint8_t Offset;
	uint8_t Flags;
	uint16_t Windown;
	uint16_t Checksum;
	uint16_t Urgenpt;
	uint8_t options[1];
};

struct tcpSession
{
	uint16_t  desPort;		//Port on the remote host
	uint16_t  srcPort;		//Port on the local host
	uint8_t desIP[4];		//IP address of the remote host
	uint8_t SeqNumber[4];	//Sequence number
	uint8_t AckNumber[4];	//Acknowlegement number
	uint8_t sesState;	//Current state of TCP session
	uint16_t  srcWin;
	uint16_t  desWin;
	uint8_t lastRxAck[4];	//Last Received Ack
	uint8_t nextAck;
	uint8_t timeout;

	uint8_t *dataBuffer; /////////////
	uint16_t dataLen;	///////////
//	struct tcpSecssion *pSession;//////////

	uint8_t appID; 		//Upper layer application ID
	uint8_t appState; 	//Upper layer application state
};

struct tcpSession tcpSessionTable[TCP_MAX_SESSION];

uint32_t TCPInitSequenceNumber(void);
void TCPCloseSession(uint8_t socketnum);
void TCPInit(void);
uint8_t TCPGetFreeSession();
uint8_t TCPCreateSession(uint16_t sourcePort,uint16_t destPort, uint8_t *dataBuffer,uint16_t dataLen,uint8_t *dstIP);
void TCPCheckTimeOut(void);
uint16_t checksum(uint8_t *buffer, uint16_t len, uint32_t csum32);
void TCPPackedSend(struct tcpSession *pSession, uint8_t Flags, uint16_t len, uint8_t *dataBuffer);
uint8_t TCPSend(uint32_t desIP, uint8_t Flags,uint8_t *dataBuffer);
void TCPProcess(uint8_t *buffer, uint16_t len);
#endif /* ETHERNET_TCP_H_ */
