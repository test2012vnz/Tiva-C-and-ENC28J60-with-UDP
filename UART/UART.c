#include "../inc.h"

void UART0_Config(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE,  GPIO_PIN_0|GPIO_PIN_1);
    UARTStdioConfig(0,115200,SysCtlClockGet());		// Use library from TI, Express config
    IntEnable(INT_UART0); 							//enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT); 		//only enable RX and TX interrupts

    clearsrc();
    UARTprintf("\tType and Enter:\n");
    UARTprintf("\t1. info   : Information.\n");
    UARTprintf("\t2. debug  : Start debug in program.\n");
    UARTprintf("\t3. arp    : Show ARP table.\n");
    UARTprintf("\t4. ping   : Ping the specified host.\n\t>>");
}

void Reset(char *pbuff){
    while(*pbuff!=0x00){
        *pbuff=0; pbuff++;}
}
static void UARTGetBuffer(char *pbuff)
{
    char c;
    if(pbuff!=0x00 && bufCount_0==0) Reset(pbuff);
    while(UARTCharsAvail(UART0_BASE)){
        c=UARTCharGetNonBlocking(UART0_BASE);
        *(pbuff+bufCount_0)=c;
        bufCount_0++;
        if(c==0x0d)
        {
        	//*(pbuff+bufCount_0-1)=0;
        	bufCount_0=0;
        }
    }
}

void UART0_IntHandler(void)
{
    UARTIntClear(UART0_BASE, UARTIntStatus(UART0_BASE, true)); //clear the asserted interrupts
    UARTGetBuffer(&UART0_buf[0]);
}
