#include "../inc.h"

void icmpEchoReply(struct ipHeader* packet)
{
	uint8_t tempIP[4]={0,0,0,0};
	uint8_t *ethFrame;
	struct icmpHeader *icmpPacket;
	icmpPacket = (struct icmpHeader*)((uint8_t*)packet + IP_HEADER_LEN);
	icmpPacket->Type = ICMP_TYPE_ECHOREPLY;
	icmpPacket->Checksum = 0;
	icmpPacket->Checksum = ipChecksum((uint8_t*)icmpPacket, HTONS(packet->Len)-IP_HEADER_LEN);
	memcpy(tempIP,packet->desIPAddr,sizeof(packet->desIPAddr));

	memcpy(packet->desIPAddr,packet->srcIPAddr,sizeof(packet->desIPAddr));
	memcpy(packet->srcIPAddr,tempIP,sizeof(packet->srcIPAddr));
	ethFrame = ((uint8_t*)packet);
	ethFrame -= ETH_HEADER_LEN;
	arpIpOut(ethFrame, 0);

	ethSendFrame(HTONS(packet->Len)+ETH_HEADER_LEN, ethFrame);
}
void icmpRequest(uint32_t dstIP, struct ipHeader* packet)				// not working
{
	uint8_t tempIP[4]={0,0,0,0};
	parseHexIP(dstIP,tempIP);
	uint8_t *ethFrame;
	struct icmpHeader *icmpPacket;
	icmpPacket = (struct icmpHeader*)((uint8_t*)packet + IP_HEADER_LEN);
	icmpPacket->Type = ICMP_TYPE_ECHOREQUEST;
	icmpPacket->Checksum = 0;
	icmpPacket->Checksum = ipChecksum((uint8_t*)icmpPacket, HTONS(packet->Len)-IP_HEADER_LEN);

	memcpy(packet->desIPAddr,tempIP,sizeof(packet->desIPAddr));
	memcpy(packet->srcIPAddr,GetInfo()->ipAddr,sizeof(packet->srcIPAddr));

	ethFrame = ((uint8_t*)packet);
	ethFrame -= ETH_HEADER_LEN;
	arpIpOut(ethFrame, 0);
	ethSendFrame(HTONS(packet->Len)+ETH_HEADER_LEN, ethFrame);
}
void icmpIpIn(struct ipHeader* packet)
{
	struct icmpHeader* icmpPacket;
	icmpPacket = (struct icmpHeader*)((uint8_t*)packet + IP_HEADER_LEN);
	// check ICMP type
	switch(icmpPacket->Type)
	{
	case ICMP_TYPE_ECHOREQUEST:
		// echo request
		icmpEchoReply(packet);
		break;
	default:
		break;
	}
}
