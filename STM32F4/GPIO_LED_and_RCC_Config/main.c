#include "stm32f407xx.h"

void SysClk_Config()
{

	RCC->CR |= 0x00010000;  		   // 16. bit of CR register = HSEON
	while(!( RCC->CR & 0x00020000 ));  // wait until HSE Ready Bit that is 17. bit of CR register
	RCC->APB1ENR |= 0x10000000;		   // PWREN= power interface clock enable
	PWR->CR      |= 0x00003000;	   	   // Voltage Regulator scale set to 1
	FLASH->ACR   |= 0x00000705;		   // Data cache enable, instruction cache enable, prefetch enable, five wait state to Flash memort access time
	RCC->CFGR     = 0x00000005  | 0x00000080    | 0x00001400  | 0x00008000  ;
				//  SW=HSE sel   HPRE=AHB div2  PPRE1=APB1 div  PPRE2=APB2 div2

}

void GPIO_Config(void)
{
	RCC->AHB1ENR   = 0x00000008;		    // GPIOD clock is enabled
	GPIOD->MODER   = 0x55000000;			// pin 12-13-14-15 are set to OUTPUT
	GPIOD->OTYPER  |= 0x00000000;			// all outputs are set to push-pull
	GPIOD->OSPEEDR |= 0x00000000;			// all outputs speed are set to low speed

}

int main(void)
{
	uint32_t time = 100000;
	SysClk_Config();
	GPIO_Config();

	while(1)
	{

		GPIOD->ODR = 0x00008000;				// PD15 is ON, others OFF
		for ( int i = 0;  i < time; i++);		// wait step

		GPIOD->ODR = 0x00004000;				// PD14 is ON, others OFF
		for ( int i = 0;  i < time; i++);

		GPIOD->ODR = 0x00002000;				// PD13 is ON, others OFF
		for ( int i = 0;  i < time; i++);

		GPIOD->ODR = 0x00001000;				// PD12 is ON, others OFF
		for ( int i = 0;  i < time; i++);

	}
}
