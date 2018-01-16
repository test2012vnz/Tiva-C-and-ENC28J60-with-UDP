
#ifndef ETHERNET_ARP_H_
#define ETHERNET_ARP_H_

#define ARP_OPCODE_REQUEST	1
#define ARP_OPCODE_REPLY	2
#define ARP_HWTYPE_ETH		1

#define ARP_TABLE_SIZE	8
#define ARP_CACHE_TIME_TO_LIVE	250

struct arpHeader{
	uint16_t	hwType;		//2				29
	uint16_t	protocol;	//2
	uint8_t		hwLen;		//1
	uint8_t		protoLen;	//2
	uint16_t	opcode;		//2
	uint8_t 	shwAddr[6];
	uint8_t 	sipAddr[4];
	uint8_t 	dhwAddr[6];
	uint8_t 	dipAddr[4];
};

struct arpEntry
{
	uint8_t ipAddr[4];
	uint8_t hwAddr[6];
	uint8_t time;		///< time to live (in ARP table); this is decremented by arpTimer()
};

void arpInit(void);
void arpSetAddress(uint8_t *ethAddr, uint8_t *ipAddr);
uint8_t arpSearchIP(uint8_t *ipaddr);
void arpUpdateEntry(uint8_t *ethAddr,uint8_t *ipAddr);
void arpPacketIn(uint8_t* ethFrame);
void arpIpOut(uint8_t* ethFrame, uint8_t *phyDstIp);
struct arpEntry *getARPMyAddr(void);
void arpProcess(uint16_t len, uint8_t* ethFrame);
void arpSendReq(uint8_t *phyDstIP, uint8_t *destMAC);
void arpTimerReq(uint8_t *phyDstIP,uint8_t timerSend);
void arpTimer(void);
void arpPrintTable(void);

#endif /* ETHERNET_ARP_H_ */
