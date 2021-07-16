#include "stm32f407xx.h"
#include "Delay.h"
#include "RCC_Config.h"

void GPIOD_Config(void)
{
	RCC->AHB1ENR   = 0x00000008;		    // GPIOD clock is enabled
	GPIOD->MODER   = 0x55000000;			// pin 12-13-14-15 are set to OUTPUT
	GPIOD->OTYPER  |= 0x00000000;			// all outputs are set to push-pull
	GPIOD->OSPEEDR |= 0x00000000;			// all outputs speed are set to low speed

}

void standbyModeEn(void)
{
	PWR->CR  |= (1<<1);			// PDDS: Power-down deepsleep 1: Enter Standby mode when the CPU enters deepsleep
	SCB->SCR |= (3<<2);
	// STM32 CORTEX M4 Instruction Set: System Control Block:system control register
	// SLEEPONEXIT; 1: Enter sleep, or deep sleep, on return from an interrupt service routine.
	// SLEEPDEEP bit;
	__WFI();                    // Wait for interrupt. Cortex M4 instruction
}
int main(void)
{
	// WKUP pin is PA0
	// Exiting Standby mode: WKUP pin rising edge, RTC alarm (Alarm A and Alarm B), RTC wakeup,
	// tamper event, time stamp event, external reset in NRST pin, IWDG reset.

	//while(!PWR_CSR_SBF);		// All registers are reset(0) after wakeup from Standby except for PWR_CR
	SysClockConfig();
    GPIOD_Config();

    PWR->CR  |= 0x00000004;     // Clear WakeUP Flag
    while(!PWR_CR_CWUF);		// Wait wakeup flag is cleared
    PWR->CSR |= 0x00000100; 	// Enable WakeUp pin : WKUP pin is used for wakeup from Standby mode and forced in input pull down
    							// configuration (rising edge on WKUP pin wakes-up the system from Standby mode).
    Delay_ms(500);
    while(1)
	{

		GPIOD->ODR = 0x00008000;				// PD15 is ON, others OFF
		Delay_ms(600);

		GPIOD->ODR = 0x00004000;				// PD14 is ON, others OFF
		Delay_ms(700);

		// SCB->SCR |= 0x00000002; 				// SLEEPONEXIT=1; SLEEPDEEP=0
		GPIOD->ODR = 0x00002000;				// PD13 is ON, others OFF
		Delay_ms(800);

		GPIOD->ODR = 0x00001000;				// PD12 is ON, others OFF
		Delay_ms(900);

		standbyModeEn();						// call the standby mode

	}
}
