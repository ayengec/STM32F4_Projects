#include "stm32f407xx.h"
#include "Delay.h"
#include "RCC_Config.h"

void GPIOD_Config(void)
{
	RCC->AHB1ENR   |= 0x00000008;		    // GPIOD clock is enabled
	GPIOD->MODER   |= 0x55000000;			// pin 12-13-14-15 are set to OUTPUT
	GPIOD->OTYPER  |= 0x00000000;			// all outputs are set to push-pull
	GPIOD->OSPEEDR |= 0x00000000;			// all outputs speed are set to low speed

}
void CS_Config(void)
{
	RCC->AHB1ENR   |= (1<<4);		        // GPIOE clock is enabled
	GPIOE->MODER   |= (1<<6);			    // PE3 set as output
	GPIOE->OTYPER  |= (0<<3);			    // PE3 is set to push-pull
	GPIOE->OSPEEDR |= (3<<7);				// PE3's speed are set to very high speed
	GPIOE->PUPDR   |= (1<<6);				// PE3 is set to pull-up

}
/*
 * ---------- SPI Master Mode Procedure --------
	1. Select the BR[2:0] bits to define the serial clock baud rate (see SPI_CR1 register).
	2. Select the CPOL and CPHA bits to define one of the four relationships between the
	data transfer and the serial clock (see Figure 248). This step is not required when the
	TI mode is selected.
	3. Set the DFF bit to define 8- or 16-bit data frame format
	4. Configure the LSBFIRST bit in the SPI_CR1 register to define the frame format. This
	step is not required when the TI mode is selected.
	5. If the NSS pin is required in input mode, in hardware mode, connect the NSS pin to a
	high-level signal during the complete byte transmit sequence. In NSS software mode,
	set the SSM and SSI bits in the SPI_CR1 register. If the NSS pin is required in output
	mode, the SSOE bit only should be set. This step is not required when the TI mode is
	selected.
	6. Set the FRF bit in SPI_CR2 to select the TI protocol for serial communications.
	7. The MSTR and SPE bits must be set (they remain set only if the NSS pin is connected
	to a high-level signal).
*/
void SPI1_Init(void)
{
	RCC->AHB1ENR |= (1<<0);  // Enable GPIO Clock

	RCC->APB2ENR |= (1<<12);  // Enable SPI1 CLock

	GPIOA->MODER |= (2<<10)|(2<<12)|(2<<14);  // Alternate functions for PA5, PA6, PA7 and Output for PA9

	GPIOA->OSPEEDR |= (3<<10)|(3<<12)|(3<<14);  // HIGH Speed for PA5, PA6, PA7, PA9

	GPIOA->AFR[0] |= (5<<20)|(5<<24)|(5<<28);   // AF5(SPI1) for PA5, PA6, PA7
    ///////////////////////////////////////

	SPI1->CR1 |= (1<<0)|(1<<1);   // CPOL=1, CPHA=1

	SPI1->CR1 |= (1<<2);  // Master Mode

	SPI1->CR1 |= (3<<3);  // BR[2:0] = 011: fPCLK/16, APB2_CLK = 84MHz, SPI clk = 5.25 MHz

	SPI1->CR1 &= ~(1<<7);  // LSBFIRST = 0, MSB first

	SPI1->CR1 |= (1<<8) | (1<<9);  // SSM=1, SSi=1 -> Software Slave Management

	SPI1->CR1 &= ~(1<<10);  // RXONLY = 0, full-duplex

	SPI1->CR1 &= ~(1<<11);  // DFF=0, 8 bit data

	SPI1->CR2 = 0;
}
// PE3 is CS pin of motion sensor

void SPI1_Enable(void)
{
	SPI1->CR1 |= (1<<6);
}

void SPI1_Disable(void)
{
	SPI1->CR1 &= ~(1<<6);
}

void CS1_Enable(void)
{
	GPIOE->BSRR = (1<<19);
}

void CS1_Disable(void)
{
	GPIOE->BSRR = (1<<3);
}

uint8_t SPI1_Receive()
{
	SPI1->DR = 0x00;
	while (!(SPI1->SR & (1<<0))); 	// Wait RXNE
	return SPI1->DR;
}
uint8_t SPI1_RegWrite(uint8_t mdata)
{
	SPI1->DR = mdata;
	while (!(SPI1->SR & (1<<0)));	// Wait RXNE
	return (SPI1->DR);
}
int main(void)
{
	SysClockConfig();
    GPIOD_Config();					// to control LEDs onboard
    CS_Config();					// PE5 is connected to sensor as Chip Select
    SPI1_Init();					// Initialize SPI1 interface
    SPI1_Enable();

	uint8_t IDdata, infoData1, infoData2;
	uint8_t xout[2], yout[2], zout[2];
	int16_t x_axis_val, y_axis_val, z_axis_val;
	float x_val, y_val, z_val;

    while(1)
	{
		GPIOD->ODR = 0x00008000;			// PD15 is ON
		Delay_ms(1000);

		CS1_Enable();
		SPI1_RegWrite(0x20);				// CTRL_REG4
		SPI1_RegWrite(0x67);				// writing data to register
	    CS1_Disable();

		CS1_Enable();
		infoData1 = SPI1_RegWrite(0x8D);	// INFO1 register
		infoData1 = SPI1_Receive(); // while receiving, sending dummy 0x00 from mosi channel
	    CS1_Disable();

		CS1_Enable();
		infoData2 = SPI1_RegWrite(0x8E);	// INFO2 register
		infoData2 = SPI1_Receive();	// while receiving, sending dummy 0x00 from mosi channel
	    CS1_Disable();

		CS1_Enable();
		IDdata = SPI1_RegWrite(0x8F);		// WHO_AM_I register
		IDdata = SPI1_Receive();	// while receiving, sending dummy 0x00 from mosi channel
	    CS1_Disable();

		CS1_Enable();
		xout[0] = SPI1_RegWrite(0xA8);	// X-axis accelerometer value burst read
		xout[0] = SPI1_Receive();	// while receiving, sending dummy 0x00 from mosi channel
		xout[1] = SPI1_Receive();	// while receiving, sending dummy 0x00 from mosi channel
	    CS1_Disable();

		CS1_Enable();
		yout[0] = SPI1_RegWrite(0xAA);	// Y-axis accelerometer value burst read
		yout[0] = SPI1_Receive();	// while receiving, sending dummy 0x00 from mosi channel
		yout[1] = SPI1_Receive();	// while receiving, sending dummy 0x00 from mosi channel
	    CS1_Disable();

		CS1_Enable();
		zout[0] = SPI1_RegWrite(0xAC);	// Z-axis accelerometer value burst read
		zout[0] = SPI1_Receive();	// while receiving, sending dummy 0x00 from mosi channel
		zout[1] = SPI1_Receive();	// while receiving, sending dummy 0x00 from mosi channel
	    CS1_Disable();

	    x_axis_val = 256*xout[1] + xout[0];
	    x_val = (2*(float)x_axis_val)/32768;

	    y_axis_val = 256*yout[1] + yout[0];
	    y_val = (2*(float)y_axis_val)/32768;

	    z_axis_val = 256*zout[1] + zout[0];
	    z_val = (2*(float)z_axis_val)/32768;

		GPIOD->ODR = 0x00004000;				// PD14 is ON
		Delay_ms(1000);

	}
}
