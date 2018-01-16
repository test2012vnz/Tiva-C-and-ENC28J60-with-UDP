
#ifndef ETHERNET_IP_H_
#define ETHERNET_IP_H_

#define IP_HEADER_LEN	20
#define IP_PROTO_ICMP	1
#define IP_PROTO_TCP	6
#define IP_PROTO_UDP	17
#define IP_TIME_TO_LIVE		128

struct ipHeader
{
	uint8_t		verHdrLen;
	uint8_t		ToS;
	uint16_t	Len;
	uint16_t	IDNumber;
	uint16_t	Offset;
	uint8_t		TTL;
	uint8_t		Protocol;
	uint16_t	Checksum;
	uint8_t 	srcIPAddr[4];
	uint8_t		desIPAddr[4];
	uint8_t		Option[4];
};


struct MyInfo
{
	uint8_t ipAddr[4];
	uint8_t subnet[4];
	uint8_t gateway[4];
	uint16_t port;
	uint8_t Broadcast[6];
};

struct MyInfo *GetInfo(void);
void ipConfig(uint8_t *myIp, uint8_t *netmask, uint8_t *gateway, uint16_t port);
uint16_t ipChecksum(uint8_t *data, uint16_t len);
void ipSend(uint32_t dstIp, uint8_t protocol, uint16_t len, uint8_t* ipData);
void IPProcess(uint16_t len, struct ipHeader *packet);

uint32_t u32con(uint8_t ip[]);
void parseHexIP(uint32_t u32_IP, uint8_t ip[]);
void SetupBasicPacket(uint8_t* packet, uint8_t protocol, uint8_t* destIP);

#endif /* ETHERNET_IP_H_ */
