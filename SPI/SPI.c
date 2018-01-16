#include "../inc.h"

/*
 * SCK 	-- PA2
 * SO 	-- PA4
 * SI 	-- PA5
 * CS 	-- PA6
 * INT 	-- PA7
 */

void spi_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);  // CS for this project is PA6 (see enc28j60.c)
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_6);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);

	GPIODirModeSet(GPIO_PORTA_BASE,  GPIO_PIN_7, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	//INT

	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet()/2, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 5000000, 8);

	GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_FALLING_EDGE);
	GPIOIntRegister(GPIO_PORTA_BASE,ISR); //------------
	GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_7);

	SSIEnable(SSI0_BASE);

	  uint32_t b;
	  while(SSIDataGetNonBlocking(SSI0_BASE, &b)) {}
}

uint8_t spi_send(uint8_t c) {
  uint32_t val;
  SSIDataPut(SSI0_BASE, c);
  SSIDataGet(SSI0_BASE, &val);
  return (uint8_t)val;
}

void ISR(){
	uint32_t status=0;

	status = GPIOIntStatus(GPIO_PORTA_BASE,true);
	GPIOIntClear(GPIO_PORTA_BASE,status);

	eth_got_frame = 1;
	IntDisable(INT_GPIOA);

  }

