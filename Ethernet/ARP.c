#include "../inc.h"

struct arpEntry ARPMyAddr;
struct arpEntry ARPTable[ARP_TABLE_SIZE];

void arpInit(void)
{
	uint8_t i;
	for(i=0; i<ARP_TABLE_SIZE; i++)
	{
		memset(ARPTable[i].ipAddr,0,sizeof(ARPTable[i].ipAddr));
		ARPTable[i].time = 0;
	}
}

void arpSetAddress(uint8_t *ethAddr, uint8_t *ipAddr)
{
	memcpy(ARPMyAddr.ipAddr,ipAddr,sizeof(ARPMyAddr.ipAddr));
	memcpy(ARPMyAddr.hwAddr,ethAddr,sizeof(ARPMyAddr.hwAddr));
}

uint8_t arpSearchIP(uint8_t *ipaddr)
{
	uint8_t i;
	for(i=0;i<ARP_TABLE_SIZE;i++)
	{
		if(u32con(ARPTable[i].ipAddr),ipaddr,sizeof(ARPTable[i].ipAddr));
			return i;
	}
	return ARP_TABLE_SIZE +1;
}

void arpUpdateEntry(uint8_t *ethAddr,uint8_t *ipAddr)
{
	uint8_t index = arpSearchIP(ipAddr);
	if(index < ARP_TABLE_SIZE)
	{
		memcpy(ARPTable[index].hwAddr,ethAddr,sizeof(ARPTable[index].hwAddr));
		memcpy(ARPTable[index].ipAddr,ipAddr,sizeof(ARPTable[index].ipAddr));
		ARPTable[index].time = ARP_CACHE_TIME_TO_LIVE;
		return;
	}
	for(index=0;index<ARP_TABLE_SIZE;index++)
	{
		if(!ARPTable[index].time)
		{
			memcpy(ARPTable[index].hwAddr,ethAddr,sizeof(ARPTable[index].hwAddr));
			memcpy(ARPTable[index].ipAddr,ipAddr,sizeof(ARPTable[index].ipAddr));
			ARPTable[index].time = ARP_CACHE_TIME_TO_LIVE;
			return;
		}
	}
}

void arpPacketIn(uint8_t* ethFrame)
{
	struct ethHeader *ethPacket = (struct ethHeader *)ethFrame;
	struct ipHeader *ipPacket = (struct ipHeader *)(ethFrame + ETH_HEADER_LEN);
	arpUpdateEntry(ethPacket->srcAddr, ipPacket->srcIPAddr);
}

void arpIpOut(uint8_t* ethFrame, uint8_t *phyDstIp)
{
	uint8_t index;
	struct ethHeader *ethPacket;
	struct ipHeader *ipPacket;
	ethPacket = (struct ethHeader *)ethFrame;
	ipPacket = (struct ipHeader *)(ethFrame + ETH_HEADER_LEN);
	ethPacket->type = HTONS(ETH_TYPE_IP);
	if(phyDstIp)
		index = arpSearchIP(phyDstIp);
	else
		index = arpSearchIP(ipPacket->desIPAddr);
	if(index <ARP_TABLE_SIZE)
	{
		memcpy(ethPacket->srcAddr,ARPMyAddr.hwAddr,sizeof(ethPacket->srcAddr));
		memcpy(ethPacket->desAddr,ARPTable[index].hwAddr,sizeof(ethPacket->desAddr));
		ethPacket->type = HTONS(ETH_TYPE_IP);
	}
	else
	{
		memcpy(ethPacket->srcAddr,ARPMyAddr.hwAddr,sizeof(ethPacket->srcAddr));
		ethPacket->desAddr[0]=0xFF;
		ethPacket->desAddr[1]=0xFF;
		ethPacket->desAddr[2]=0xFF;
		ethPacket->desAddr[3]=0xFF;
		ethPacket->desAddr[4]=0xFF;
		ethPacket->desAddr[5]=0xFF;
		ethPacket->type = HTONS(ETH_TYPE_IP);
	}
}

struct arpEntry *getARPMyAddr(void)
{
	return &ARPMyAddr;
}

void arpProcess(uint16_t len, uint8_t* ethFrame)
{
	struct ethHeader *ethPacket;
	struct arpHeader *arpPacket;
	ethPacket = (struct ethHeader *)ethFrame;
	arpPacket = (struct arpHeader *)(ethFrame + 14);

	if(	(arpPacket->hwType == 0x0100) &&
		(arpPacket->protocol == 0x0008) &&
		(arpPacket->hwLen == 0x06) &&
		(arpPacket->protoLen == 0x04) &&
		u32con(arpPacket->dipAddr)== u32con(ARPMyAddr.ipAddr))
	{
		if(arpPacket->opcode == HTONS(ARP_OPCODE_REQUEST))
		{
			arpUpdateEntry(arpPacket->shwAddr, arpPacket->sipAddr);
			memcpy(arpPacket->dhwAddr,arpPacket->shwAddr,sizeof(arpPacket->dhwAddr));
			memcpy(arpPacket->dipAddr,arpPacket->sipAddr,sizeof(arpPacket->dipAddr));
			memcpy(arpPacket->shwAddr,getARPMyAddr()->hwAddr,sizeof(arpPacket->shwAddr));
			memcpy(arpPacket->sipAddr,getARPMyAddr()->ipAddr,sizeof(arpPacket->sipAddr));
			arpPacket->opcode = HTONS(ARP_OPCODE_REPLY);
			memcpy(ethPacket->desAddr,ethPacket->srcAddr,sizeof(ethPacket->desAddr));
			memcpy(ethPacket->srcAddr,getARPMyAddr()->hwAddr,sizeof(ethPacket->srcAddr));

			ethSendFrame(len, (uint8_t*)ethPacket);
			return;
		}
		if(arpPacket->opcode == HTONS(ARP_OPCODE_REPLY))
		{
			arpUpdateEntry(arpPacket->shwAddr, arpPacket->sipAddr);
		}
	}
}
void arpSendReq(uint8_t *phyDstIP, uint8_t *destMAC)
{
	struct ethHeader *ethPacket;
	struct arpHeader *arpPacket;
	ethPacket = (struct ethHeader *)ethGetBuffer();
	arpPacket = (struct arpHeader *)(ethGetBuffer()+14);

	ethPacket->type = HTONS(ETH_TYPE_ARP);

//	if((arpSearchIP(phyDstIP) <ARP_TABLE_SIZE)&&phyDstIP)
//	{
//		memcpy(ethPacket->srcAddr,ARPMyAddr.hwAddr,sizeof(ethPacket->srcAddr));
//		memcpy(ethPacket->desAddr,ARPTable[arpSearchIP(phyDstIP)].hwAddr,sizeof(ethPacket->desAddr));
//	}
//	else
//	{
		memcpy(ethPacket->srcAddr,ARPMyAddr.hwAddr,sizeof(ethPacket->srcAddr));
		ethPacket->desAddr[0]=destMAC[0];
		ethPacket->desAddr[1]=destMAC[1];
		ethPacket->desAddr[2]=destMAC[2];
		ethPacket->desAddr[3]=destMAC[3];
		ethPacket->desAddr[4]=destMAC[4];
		ethPacket->desAddr[5]=destMAC[5];
	//}
	arpPacket->hwType = HTONS(1);
	arpPacket->protocol = HTONS(0x0800);
	arpPacket->hwLen = 0x06;
	arpPacket->protoLen = 0x04;
	arpPacket->opcode = HTONS(1);
	memcpy(arpPacket->shwAddr,getARPMyAddr()->hwAddr,sizeof(arpPacket->shwAddr));
	memcpy(arpPacket->sipAddr,getARPMyAddr()->ipAddr,sizeof(arpPacket->sipAddr));
	memset(arpPacket->shwAddr, 0, sizeof(arpPacket->shwAddr));
	memcpy(arpPacket->dipAddr,phyDstIP,sizeof(arpPacket->dipAddr));
	ethSendFrame(42, (uint8_t *)ethPacket);
}
void arpTimerReq(uint8_t *phyDstIP,uint8_t timerSend)
{

}
void arpTimer(void)
{
	uint8_t index;
	for(index=0;index<ARP_TABLE_SIZE;index++)
	{
		if(ARPTable[index].time)
			ARPTable[index].time--;
	}
}
void arpPrintTable(void)
{
	uint8_t i;
	UARTprintf("\tTime\tMAC Address\t\t  IP Address\n");
	UARTprintf("\t---------------------------------------------\n");
	for(i=0;i<ARP_TABLE_SIZE;i++)
	{
		if(ARPTable[i].time)
		{
			UARTprintf("\t%d\t",ARPTable[i].time);
			UARTprintf("%x.%x.%x.%x.%x.%x\t",ARPTable[i].hwAddr[0],ARPTable[i].hwAddr[1],ARPTable[i].hwAddr[2],ARPTable[i].hwAddr[3],
													ARPTable[i].hwAddr[4],ARPTable[i].hwAddr[5]);
			UARTprintf("%d.%d.%d.%d\n",ARPTable[i].ipAddr[0],ARPTable[i].ipAddr[1],ARPTable[i].ipAddr[2],ARPTable[i].ipAddr[3]);
		}
	}
}
