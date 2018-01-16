#include "../inc.h"
 char *infoHeader[5]={"\tType and Enter:\n",
		 	 	 	 "\t1. info   : Information.\n",
					 "\t2. debug  : Start debug in program.\n",
					 "\t3. arp    : Show ARP table.\n",};
 //uint8_t routerMAC[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t targetip[] = {169,254,38,96};
bool SearchIndexOf(char src[], char str[])
{
   int i, j, firstOcc;
   i = 0, j = 0;

   while (src[i] != '\0')
   {

      while (src[i] != str[0] && src[i] != '\0')
         i++;

      if (src[i] == '\0') return 0;

      firstOcc = i;

      while (src[i] == str[j] && src[i] != '\0' && str[j] != '\0')
      {
         i++;
         j++;
      }
      if (str[j] == '\0') return 1;
      if (src[i] == '\0') return 0;

      i = firstOcc + 1;
      j = 0;
   }

   return 0;
}

void clearsrc(void)
{
	UARTCharPut(UART0_BASE, 27);
	UARTprintf("\033[2J");

	UARTCharPut(UART0_BASE, 27);
	UARTprintf("[H");
}

void debug(void)
{
	uint8_t i;
	if(UART0_buf[0]!=0x00)
	{
		if(UART0_buf[0]==0x0d)
		{
			clearsrc();
			UART0_buf[0]=0;
			for(i=0;i<4;i++)
				UARTprintf(infoHeader[i]);
			//TCPSend(u32con(targetip),TCP_NON_FLAG,"nam");
		//	arpSendReq(targetip,GetInfo()->Broadcast);
;		}
		if(SearchIndexOf(UART0_buf, "info"))
		{
			clearsrc();
			UARTprintf("\tMy information:\n");
			UARTprintf("\tMAC Address....: %x.%x.%x.%x.%x.%x\n",getARPMyAddr()->hwAddr[0],getARPMyAddr()->hwAddr[1],getARPMyAddr()->hwAddr[2],
																getARPMyAddr()->hwAddr[3],getARPMyAddr()->hwAddr[4],getARPMyAddr()->hwAddr[5]);
			UARTprintf("\tIP Address.....: %d.%d.%d.%d\n",GetInfo()->ipAddr[0],GetInfo()->ipAddr[1],GetInfo()->ipAddr[2],GetInfo()->ipAddr[3]);
			UARTprintf("\tSubnet Mask....: %d.%d.%d.%d\n",GetInfo()->subnet[0],GetInfo()->subnet[1],GetInfo()->subnet[2],GetInfo()->subnet[3]);
			UARTprintf("\tGateway........: %d.%d.%d.%d\n",GetInfo()->gateway[0],GetInfo()->gateway[1],GetInfo()->gateway[2],GetInfo()->gateway[3]);
			UARTprintf("\tPort...........: %u\n",GetInfo()->port);
			Reset(UART0_buf);
		}
		if(SearchIndexOf(UART0_buf, "debug"))
		{
			clearsrc();
			UARTprintf("\tStart debug in progran:\n");
			Reset(UART0_buf);
		}
		if(SearchIndexOf(UART0_buf, "arp"))
		{
			clearsrc();
			arpPrintTable();
			Reset(UART0_buf);
		}
		if(SearchIndexOf(UART0_buf, "ping "))
		{
			clearsrc();
			Reset(UART0_buf);
		}
	}
}

