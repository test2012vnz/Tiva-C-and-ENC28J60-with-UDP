#include "../inc.h"

void add_data(uint8_t *opt,uint8_t data[], uint16_t len)
{
	uint16_t i;
	for(i=0;i<len;i++)
		*opt++ = data[i];
}

void udpSend(uint32_t dstIp, uint16_t dstPort, uint16_t srcPort, uint16_t len, uint8_t* udpData)
{
	struct udpHeader *udpPacket;
	udpPacket = (struct udpHeader*)(udpData - UDP_HEADER_LEN);
	len += UDP_HEADER_LEN;

	udpPacket->desPort = HTONS(dstPort);
	udpPacket->srcPort  = HTONS(srcPort);
	udpPacket->Len = HTONS(len);
	udpPacket->Checksum = 0;
	ipSend(dstIp, IP_PROTO_UDP, len, (uint8_t*)udpPacket);
}

void UDPProcess(uint16_t len, struct ipHeader* packet)
{
	struct udpHeader *udpPacket;
	udpPacket = (struct udpHeader *)((uint8_t*)packet + IP_HEADER_LEN);
	if(udpPacket->srcPort == HTONS(DHCP_UDP_SERVER_PORT) && udpPacket->desPort == HTONS(DHCP_UDP_CLIENT_PORT))
		dhcpIn((len - IP_HEADER_LEN - UDP_HEADER_LEN), (struct DHCPHeader*)((uint8_t*)packet + IP_HEADER_LEN + UDP_HEADER_LEN));
	if(udpPacket->desPort == HTONS(GetInfo()->port) && u32con(packet->desIPAddr) == u32con(GetInfo()->ipAddr))
	{
		memset(udpBuffer,0,MAXPACKETLEN);
		UDPParse_msg(packet);
		UDPReply(u32con(packet->srcIPAddr), 23,"Hello\n");
	}
}

void UDPReply(uint32_t dstIP, uint16_t dstPort,uint8_t *str)
{
	struct ipHeader * ippacket = (struct ipHeader*)&ethBuffer[ETH_HEADER_LEN];
	struct udpHeader *udppacket = (struct udpHeader *)((uint8_t*)ippacket + IP_HEADER_LEN);
	udppacket->desPort = HTONS(dstPort);
	udppacket->srcPort = HTONS(GetInfo()->port);
	udppacket->Len = HTONS(UDP_HEADER_LEN + strlen((char *)str));
	udppacket->Checksum =0;
	add_data(udppacket->data, str, strlen((char *)str));
	ipSend(dstIP, IP_PROTO_UDP, UDP_HEADER_LEN + strlen((char *)str), (uint8_t*)udppacket);
}
void UDPParse_msg(struct ipHeader* packet)
{
	uint16_t len;
	len = HTONS(packet->Len) -IP_HEADER_LEN - UDP_HEADER_LEN;
	uint8_t *end = (uint8_t *)(packet) + IP_HEADER_LEN + UDP_HEADER_LEN;
	memcpy(udpBuffer,end, len);
}
