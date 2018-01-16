#include "../inc.h"

struct MyInfo Info;

struct MyInfo *GetInfo(void)
{
	return &Info;
}

void ipConfig(uint8_t *myIp, uint8_t *netmask, uint8_t *gateway, uint16_t port)
{
	uint8_t routerMAC[]= {0xff,0xff,0xff,0xff,0xff,0xff};
	memcpy(GetInfo()->ipAddr,myIp,sizeof(GetInfo()->ipAddr));
	memcpy(GetInfo()->subnet,netmask,sizeof(GetInfo()->subnet));
	memcpy(GetInfo()->gateway,gateway,sizeof(GetInfo()->gateway));
	GetInfo()->port = port;
	memcpy(GetInfo()->Broadcast,routerMAC,sizeof(GetInfo()->Broadcast));
}
uint16_t ipChecksum(uint8_t *data, uint16_t len)
{
    uint32_t sum = 0;
    for (;;) {
        if (len < 2)
            break;
		sum += *((uint16_t *)data);
		data+=2;
        len -= 2;
    }
    if (len)
        sum += *(unsigned char *) data;

    while ((len = (uint16_t) (sum >> 16)) != 0)
        sum = (uint16_t) sum + len;

    return (uint16_t) sum ^ 0xFFFF;
}

void ipSend(uint32_t dstIp, uint8_t protocol, uint16_t len, uint8_t* ipData)
{
	uint8_t ip_temp[]={0,0,0,0};
	parseHexIP(dstIp, ip_temp);

	struct ethHeader *ethPacket;
	struct ipHeader *ipPacket;
	ipPacket = (struct ipHeader *)(ipData - IP_HEADER_LEN);
	ethPacket = (struct ethHeader *)(ipData - IP_HEADER_LEN - ETH_HEADER_LEN);
	len += IP_HEADER_LEN;

	memcpy(ipPacket->desIPAddr,ip_temp,sizeof(ipPacket->desIPAddr));
	memcpy(ipPacket->srcIPAddr,GetInfo()->ipAddr,sizeof(ipPacket->srcIPAddr));
	ipPacket->Protocol = protocol;
	ipPacket->Len = HTONS(len);
	ipPacket->verHdrLen = 0x45;
	ipPacket->ToS =0;
	ipPacket->IDNumber = 0;
	ipPacket->Offset = 0;
	ipPacket->TTL = IP_TIME_TO_LIVE;
	ipPacket->Checksum = 0;
	ipPacket->Checksum = ipChecksum((uint8_t*)ipPacket, IP_HEADER_LEN);

	if(u32con(GetInfo()->subnet)&dstIp == u32con(GetInfo()->ipAddr)&u32con(GetInfo()->subnet))
		arpIpOut((uint8_t *)ethPacket, 0);										//local
	else
		arpIpOut((uint8_t *)ethPacket, GetInfo()->gateway);

	len += ETH_HEADER_LEN;
	ethSendFrame(len, (uint8_t*)ethPacket);
}
void IPProcess(uint16_t len, struct ipHeader *packet)
{
	if(	u32con(packet->desIPAddr) != u32con(GetInfo()->ipAddr) &&
		u32con(packet->desIPAddr) != (u32con(GetInfo()->ipAddr)|u32con(GetInfo()->subnet)) &&
		u32con(packet->desIPAddr)!=0xFFFFFFFF)
		return;
	if(packet->Protocol == IP_PROTO_ICMP)
	{
		//UARTprintf("Recive ICMP packet\n");
		icmpIpIn((struct ipHeader*)packet);
	}
	else if(packet->Protocol == IP_PROTO_UDP)
	{
		//UARTprintf("Recive UDP packet\n");
		UDPProcess(len, ((struct ipHeader *)packet));
	}
	else if(packet->Protocol == IP_PROTO_TCP)
	{
		//UARTprintf("Recive TCP packet\n");
		TCPProcess((uint8_t*)packet,len-((packet->verHdrLen & 0x0F)<<2));
	}

}
uint32_t u32con(uint8_t ip[])
{
	return ((uint32_t)((uint8_t)ip[0])<<24)+((uint32_t)((uint8_t)ip[1])<<16)+((uint32_t)((uint8_t)ip[2])<<8)+(uint8_t)ip[3];//((a<<24)|(b<<16)|(c<<8)|(d))
}

void parseHexIP(uint32_t u32_IP, uint8_t ip[])
{
	ip[0] = (uint8_t)(u32_IP>>24);
	ip[1] = (uint8_t)(u32_IP>>16);
	ip[2] = (uint8_t)(u32_IP>>8);
	ip[3] = (uint8_t)(u32_IP);
}
