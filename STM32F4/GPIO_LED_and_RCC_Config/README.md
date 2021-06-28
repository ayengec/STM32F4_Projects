

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.st.com%2Fen%2Fevaluation-tools%2Fstm32f0discovery.html&psig=AOvVaw2kMIZO_3YmJXV29iF31QR-&ust=1624441319916000&source=images&cd=vfe&ved=0CAoQjRxqFwoTCJjMnu75qvECFQAAAAAdAAAAABAD" alt="Logo" width="100" height="180">
  </a>

  <h3 align="center">General Purpose Input Output Interface</h3>

  <p align="center">
    GPIO OUTPUT with Register Level Programming
    <br />
    <a href="https://github.com/ayengec/Microcontroller-Projects/issues">Report Bug</a>
    ·
    <a href="https://github.com/ayengec/Microcontroller-Projects/issues">Request Feature</a>
  </p>
</p>

<!-- ABOUT THE PROJECT -->
## Project Summary
This project includes two seperate main.c code to drive GPIO LEDs on STM32F407VG-DISC1 development board. These main codes are coded with HAL library and bare metal register level. 


### Built With
This section should list any major frameworks that you built your project using. Leave any add-ons/plugins for the acknowledgements section. Here are a few examples.
* [STM32CubeMX IDE](https://www.st.com/en/development-tools/stm32cubeide.html)
* [VSCode](https://code.visualstudio.com)

<!-- GETTING STARTED -->
## 1. Clock Configuration
We need to set clock configuration about GPIOD port on STM32F407VG-DISC1 board
![image](https://github.com/ayengec/Microcontroller-Projects/blob/main/STM32F4/GPIO_LED_and_RCC_Config/clk_config.PNG)

## 2. GPIO Configuration
STM32F4 Board has 4 LEDs on-board as PD12-PD13-PD14-PD15. The possible GPIO configurations are shown below:
![image](https://github.com/ayengec/Microcontroller-Projects/blob/main/STM32F4/GPIO_LED_and_RCC_Config/gpio_diagram.PNG)

To use these pins as output, we must set the configuration as written in reference manual:
![image](https://github.com/ayengec/Microcontroller-Projects/blob/main/STM32F4/GPIO_LED_and_RCC_Config/output_configuration.PNG)

So our GPIO_Config() function sets the configuration our pins declared in reference manual:

Firstly; we set the MODER register of GPIOD as how to use them.
![image](https://github.com/ayengec/Microcontroller-Projects/blob/main/STM32F4/GPIO_LED_and_RCC_Config/1-GPIO_MODER.png)

After we set the (OUTPUT TYPE) OTYPER register whether Push-Pull or Open-Drain.
![image](https://github.com/ayengec/Microcontroller-Projects/blob/main/STM32F4/GPIO_LED_and_RCC_Config/2-GPIO_OTYPER.PNG)

Third step; we can set Output Speed on OSPEEDR register based on power consumption.
![image](https://github.com/ayengec/Microcontroller-Projects/blob/main/STM32F4/GPIO_LED_and_RCC_Config/3-GPIO_OSPEEDR.png)

After that, we can set whether the pin has internal pull-op or pull-down.
![image](https://github.com/ayengec/Microcontroller-Projects/blob/main/STM32F4/GPIO_LED_and_RCC_Config/4-GPIO_PUPDR.png)

In final step, we can set the default values of our output ports. In our main program, we will set this registers HIGH or LOW within time domain.
![image](https://github.com/ayengec/Microcontroller-Projects/blob/main/STM32F4/GPIO_LED_and_RCC_Config/5-GPIO_ODR.PNG)






### To clone project
Clone the repo
   ```sh
   git clone https://github.com/ayengec/Microcontroller-Projects
   ```
