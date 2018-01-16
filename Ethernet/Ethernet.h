
#ifndef ETHERNET_ETHERNET_H_
#define ETHERNET_ETHERNET_H_

#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IP  0x0800
#define ETH_TYPE_IP6 0x86dd

#define		MTU_SIZE	1460
#define ETH_HEADER_LEN	14
#define IP_HEADER_LEN	20
#define TCP_HEADER_LEN	20
#define ETHERNET_BUFFER_SIZE		(700+ETH_HEADER_LEN+IP_HEADER_LEN+TCP_HEADER_LEN)

struct ethHeader{
	uint8_t desAddr[6];
	uint8_t srcAddr[6];
	uint16_t type;
};

uint8_t ethBuffer[ETHERNET_BUFFER_SIZE];
void ethInit(uint8_t* macaddr);
void ethSendFrame(uint16_t len, uint8_t* packet);
uint16_t ethGetFrame(uint16_t maxlen, uint8_t* packet);
uint8_t* ethGetBuffer(void);
void PacketInComming(void);

#endif /* ETHERNET_ETHERNET_H_ */
