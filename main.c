#include "inc.h"


void netInit(uint8_t* macaddr,uint8_t *ipaddress, uint8_t *netmask, uint8_t *gatewayip, uint16_t port);

uint8_t mac_addr[] 	= {0x02, 0x00, 0xC0, 0xA8, 0x00, 0x01 };
uint8_t ip_addr[] 	= {0,0,0,0};
uint8_t subnet[] 	= {0,0,0,0};
uint8_t gateway[] 	= {0,0,0,0};
uint16_t port		= 1999;
uint32_t c=0;
uint8_t abc[] 	= {0,0,0,0};
int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	UART0_Config();
    InitTime();
    Timer_Config();
    spi_init();

    parseHexIP(c+1, abc);
    netInit(mac_addr, ip_addr, subnet, gateway, port);
	while(1)
	{
		debug();
		if(DhcpLeaseTime == 0)
				dhcpService();
		PacketInComming();
	}
}

void netInit(uint8_t* macaddr,uint8_t *ipaddress, uint8_t *netmask, uint8_t *gatewayip, uint16_t port)
{
	ethInit(macaddr);
	ipConfig(ipaddress, netmask, gatewayip, port);
	arpInit();
	arpSetAddress(macaddr, ipaddress);
	TCPInit();
	if(u32con(GetInfo()->ipAddr)==0x00000000)
	{
		UARTprintf("\nGetting IP address...\n");
		dhcpInit();
	}
}
