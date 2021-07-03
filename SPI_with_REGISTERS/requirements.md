SPI_Config()
  1.Enable SPI clock
    RCC_APB2->SPI1 EN
  2.Configure the Control Register RC1
    CPHA and CPOL = 1       0. and 1. bits
    MSTR                    2. bit = master mode selection
    baudrate                APB2clk/div => 84MHZ / 64  == 1,3125 MHz
    SPI Enable => in main.c
    LSBFirst => 0=MSBFirst
    RXONLY =>   0= full duplex
    DFF => 0 = 8bit format
    SSM=> Software slave management enable   SSI=>enable
  3.Configure the RC2
    CR2->0x00000000
    
GPIO_Config()
  //SPI1_CK   = A5  alternate func
  //SPI1_MISO = A6  alternate func
  //SPI1_MOSI = A7  alternate func
  // we selected A2 is CS GPIO output mode!
  AHB1_ENR = GPIOA enable
  GPIOA_MODER => pin 5-6-7 select alternate function
  GPIOA_OTYPER=> push pull
  GPIOA_OSPEEDR => 11 for every SPI pins to HIGH speed
  GPIOA_AFR => AF5 for each A7/A6/A5         // Table 26AFRL AF5 = SPI
SPI_enable()
  SPI1_CR1 = write 1 to 6. bit
SPI_disable()
  SPI1_CR1 = write 0 to 6. bit
CS_Enable()
  GPIOA=>ODR Write 1 Pin A2
CS_Disable()
  GPIOA=>ODR Write 0 Pin A2
  /////////////////////////////////////////////////////////////
SPI_Send(uint8_t *data, int size)
  // Wait TXE bit is 1 SPI1_SR  Stats Register
  // Write data to SPI1_DR Data register
  // Wait Busy bit until 0
  // Clear overrun flag by reading DR and SR
  for(int i=0; i<size, i++)
  {
      while(!(SPI1->SR) & 0x2);  // waits TXE bit == 1
      SPI1->DR = data[i]          // load data to Data register
      i++
  }
  while(!(SPI1->SR & 0x2))  // waits TXE bit == 1
  while(SPI1->SR & 0x80)    // wait the Busy flag == 0
  // transmitting process is done
  
  //clearing overrung flag
  char r_data = SPI1->DR
       r_data = SPI1->SR
  /////////////////////////////////////////////////////////////
  SPI_Read(uint8_t *data, int size)
      for(int i=size; i==0, i--)
    {
      while(SPI1->SR & 0x80);  // waits BSY is set to 0
      SPI1->DR = 0             // write dummy data
      while(!(SPI1->SR & 0x1))  // waits RXNE is 1
      *data++ = SPI1->DR      
    }
  
  to main.c
  https://www.youtube.com/watch?v=ke0LC6qsTsw&list=PLfIJKC1ud8gjBJVXgrhKifRAYUk9PPcbf&index=4
