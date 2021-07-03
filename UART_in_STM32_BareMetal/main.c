#include "RCC_Config.h"
#include "Delay.h"

void uart2_config(void)
{
	/*
	 *  1. Enable the USART by writing the UE bit in USART_CR1 register to 1.
		2. Program the M bit in USART_CR1 to define the word length.
		3. Program the number of stop bits in USART_CR2.
		4. Select DMA enable (DMAT) in USART_CR3 if Multi buffer Communication is to take
		place. Configure the DMA register as explained in multibuffer communication.
		5. Select the desired baud rate using the USART_BRR register.
		6. Set the TE bit in USART_CR1 to send an idle frame as first transmission.
		7. Write the data to send in the USART_DR register (this clears the TXE bit). Repeat this
		for each data to be transmitted in case of single buffer.
		8. After writing the last data into the USART_DR register, wait until TC=1. This indicates
		that the transmission of the last frame is complete. This is required for instance when
		the USART is disabled or enters the Halt mode to avoid corrupting the last
		transmission.
	*/
	// 1 - CLOCK ENABLE
	// 	USART2 is connected to APB1 bus; we need to enable USART2 clock on APB1 control registers,
	// 	we can see in datasheet of stm32f40xx architecture, Figure 5.
	// 	Moreover, we need to enable AHB1 for GPIOA ports will be using as UART ports
	// 	USART2 TX -> PORTA.2   USART2 RX -> PORTA.3

	RCC->AHB1ENR |= (1<<0 );  // GPIOA clock enable, 0.bit of RCC AHB1 register
	RCC->APB1ENR |= (1<<17);  // USART2 clock enable, 17.bit of RCC APB1 register

	// 2 - GPIO port configuration as UART mode in GPIOA
	GPIOA->MODER   |= 0x000000A0; // 7-6. bits = 2'b10 => alternate function <= 5-4.bits = 2'b10
	GPIOA->OSPEEDR |= 0x000000F0; // 7-6. bits = 2'b11 => very high-speed <= 5-4.bits = 2'b11
	GPIOA->AFR[0]  |= 0x00007700; // To write port3 and 2 AF7=0111;  AFRL3 and AFRL2 fields:
								  // In figure 26, we select the appropriate mode of Alternate function.
								  // But there is double word AFRH and AFRL for the configuration
								  // They are defined in stm32f407xx.h as AFR[0]=0x20base and AFR[1]=0x24base
	// 3 - UART ENABLE and select data bits
	USART2->CR1 |= 0x00000000;  // clearing after system reset, 13.bit as UE usart enabled ? or disabled
	USART2->CR1 |= 0x00002000;  // M field: 0=8bit data


	// 4 - BAUDRATE CONFIGURATION
	// Remember the APB1 clock is set to 4 MHz
	/* TX/RX BAUD = fckapb1 / (8*(2-over8)*usartdiv)
	 * to set 115200 baud = 42*10e6 / ((2-0)*8*usartdiv)  usartdiv=22.7864 so we need to write 2
	 * 22 and 16*0.7864 = 13
	 * */
	USART2->BRR = (13<<0) | (22<<4);   // Baud rate of 115200, PCLK1 at 45MHz
	// 5 - RX and TX modes ENABLE
	USART2->CR1 |= 0x0000000C;	// 3.bit =1 TXEN         2.bit=1 RXEN

  USART2->CR2 |= (0<<13) | (0<<12);  // 1 bit stop bit setting

}

void uart2_sendchar(char m_data)
	/* USART2_DR data register  24bit reserved, last 8 bits as data
	 * This register is used to received data buffer or being transmitted data
	 * after send this data, wait until Transmission complete bit is set.
	 * We can monitor this in the USART2_SR = status register
	 * */

{

/*	  // load the data into data register
	while(!(USART2->SR & 0x40))  // wait until 6.bit of the status register equals to '1'
	{
		USART2->DR |= m_data;
	}*/
	USART2->DR = m_data;
	while (!(USART2->SR & (1<<6)));
}

void uart2_sendString(char *m_string)
{
	while(*m_string) uart2_sendchar(*m_string++);
}

char uart2_receiveChar(void)
	// We need to wait until RXNE field is set = READ DATA REGISTER IS NOT EMPTY
	// if Received data is ready to be read = 1
	// after RXNE is set, we could read data from USART2_DR register
{
	char r_data;

	while(USART2->SR & 0x20);
	r_data = USART2->DR;
	return r_data;
}
void LEDs_config(void)
{
	RCC->AHB1ENR   |= 0x00000008;		    // GPIOD clock is enabled
	GPIOD->MODER   |= 0x55000000;			// pin 12-13-14-15 are set to OUTPUT
	GPIOD->OTYPER  |= 0x00000000;			// all outputs are set to push-pull
	GPIOD->OSPEEDR |= 0x00000000;			// all outputs speed are set to low speed
}


int main(void)
{
	//char m_rcv_data;

	SysClockConfig();
	LEDs_config();
	TIM6Config();

	uart2_config();

	Delay_ms(400);
	GPIOD->ODR = 0x00000000;				// PD15 is ON, others OFF


	while(1)
	{
		uart2_sendchar('S');
		uart2_sendchar('T');
		uart2_sendchar('A');
		uart2_sendchar('R');
		uart2_sendchar('T');

		Delay_us(50);

		uart2_sendString("ayengec\n");
		GPIOD->ODR = 0x00008000;				// PD15 is ON, others OFF
		Delay_ms(10);							// wait step

		GPIOD->ODR = 0x00006000;				// PD15 is ON, others OFF
		Delay_ms(1000);							// wait step

		//char m_rcv_data = uart2_receiveChar();
		//Delay_ms(500);
		//uart2_sendString("YOU SENT: ");
		//uart2_sendchar(m_rcv_data);  // CHECK!!!
	}
}
