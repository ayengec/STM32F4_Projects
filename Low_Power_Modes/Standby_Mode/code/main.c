// ayengec STM32 Vendor Specific Standbymode Example with registers

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

	// WKUP pin is PA0
	// Exiting Standby mode: WKUP pin rising edge, RTC alarm (Alarm A and Alarm B), RTC wakeup,
	// tamper event, time stamp event, external reset in NRST pin, IWDG reset.
	PWR->CR  |= 0x00000004;     // Clear WakeUP Flag
	while(!PWR_CR_CWUF);		// Wait wakeup flag is cleared
	PWR->CSR |= 0x00000100; 	// Enable WakeUp pin : WKUP pin is used for wakeup from Standby mode and forced in input pull down
	    						// configuration (rising edge on WKUP pin wakes-up the system from Standby mode).

	//**************************** Standby configuration ****************************

	// STM32 CORTEX M4 Instruction Set: System Control Block:system control register
	// SLEEPONEXIT; 1: Enter sleep, or deep sleep, on return from an interrupt service routine.
	// sleeponexit automatically forces to low power mode! it blocks to process multithread!
	// SLEEPDEEP bit;    if sleepdeep ==1 Deep Sleep mode, else normal sleep mode
	PWR->CR  |= (1<<1);			// PDDS: Power-down deepsleep 1: Enter Standby mode when the CPU enters deepsleep
	SCB->SCR |= (1<<2)|(1<<1);	// SLEEPDEEP =1, SLEEPONEXIT=1
	__WFI();                    // Wait for interrupt. Cortex M4 instruction, entering standby mode
}
int main(void)
{

	SysClockConfig();
    GPIOD_Config();
    RCC->APB1ENR |= (1<<28);	// PWREN: Power interface clock enable

    Delay_ms(500);
    while(1)
	{

		GPIOD->ODR = 0x00008000;				// PD15 is ON, others OFF
		Delay_ms(600);

		GPIOD->ODR = 0x00004000;				// PD14 is ON, others OFF
		Delay_ms(700);

		GPIOD->ODR = 0x00002000;				// PD13 is ON, others OFF
		Delay_ms(800);

		GPIOD->ODR = 0x00001000;				// PD12 is ON, others OFF
		Delay_ms(900);

		standbyModeEn();						// call the standby mode

	}
}
