#include "../inc.h"

uint8_t i=0;
static volatile uint32_t counter;

static void SycTickHandler(void) {
    counter++;
}

void InitTime(void) {
    SysTickPeriodSet(SysCtlClockGet() / 1000000UL); // 1000 for milliseconds & 1000000 for microseconds
    SysTickIntRegister(SycTickHandler);
    SysTickIntEnable();
    SysTickEnable();

}

void delay(uint32_t ms) {
    delayMicroseconds(ms * 1000UL);
}

void delayMicroseconds(uint32_t us) {
    uint32_t start = micros();
    while ((int32_t)(micros() - start) < us) {
        // Do nothing
    };
}

uint32_t millis(void) {
    return counter / 1000UL;
}

uint32_t micros(void) {
    return counter;
}
void Timer_ISR(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	i++;

	if(i==10)
	{
		arpTimer();
		i=0;
	}
	dhcpTimer();
}
void Timer_Config(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()-1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);
}
