#include "../inc.h"
uint8_t ethBuffer[ETHERNET_BUFFER_SIZE];

void ethInit(uint8_t* macaddr)
{
	enc28j60Init(macaddr);
	IntEnable(INT_GPIOA);
}

void ethSendFrame(uint16_t len, uint8_t* packet)
{
	enc28j60PacketSend(len, packet);
}

uint16_t ethGetFrame(uint16_t maxlen, uint8_t* packet)
{
	return enc28j60PacketReceive(maxlen, packet);
}

uint8_t* ethGetBuffer(void)
{
	return ethBuffer;
}

void PacketInComming(void)
{
	uint16_t len;
	struct ethHeader *ethPacket;
//	if(!eth_got_frame) {
//		IntEnable(INT_GPIOA);
//		return;
//	}
    eth_got_frame = 0;
    len = ethGetFrame(ETHERNET_BUFFER_SIZE, ethBuffer);
    if(len)
    {
    	IntDisable(INT_GPIOA);
    	ethPacket = (struct ethHeader *)&ethBuffer[0];
    	if(ethPacket->type==HTONS(ETH_TYPE_IP))
    	{
    		IPProcess(len, (struct ipHeader*)&ethBuffer[ETH_HEADER_LEN]);
    	}
    	else if(ethPacket->type==HTONS(ETH_TYPE_ARP))
    	{
    		//UARTprintf("\nNhan goi ARP");
    		arpProcess(len-ETH_HEADER_LEN, ethBuffer);
    	}
    }
	IntEnable(INT_GPIOA);
}
