/*
 * lcd_7in.c
 *
 *  Created on: Feb 8, 2023
 *      Author: gunter3
 */

#include "stm32f0xx.h"
#include <stdint.h>
#include "lcd_7in.h"
#include "RA8775_commands.h"
#include "gui.h"

//int counter = 0;
uint8_t _textScale;
//Button button1;
volatile uint8_t canTouch = 1;

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

/****************************************************************************
 *                              SPI FUNCTIONS                               *
 ****************************************************************************/

// setup GPIOB pins for display and setup spi1 to interface with the display
void setup_spi1() {
	// enable RCC clock to GPIO B Ports
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// clear and set the MODER values for PB8,9,14 for outputs (01 in MODER)
	// PB11 cleared and stays 0 to be input
	GPIOB->MODER &= ~(GPIO_MODER_MODER8 /*| GPIO_MODER_MODER9*/| GPIO_MODER_MODER11 | GPIO_MODER_MODER14);
	GPIOB->MODER |= (GPIO_MODER_MODER8_0 /*| GPIO_MODER_MODER9_0*/ | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0);
	// clear and set the MODER for PB3,4,5 for alternate functions (10 in MODER)
	GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
	GPIOB->MODER |= (GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1| GPIO_MODER_MODER5_1);

	// sets the bits for PB8,9,11,14 for the ODR
	GPIOB->ODR |= (GPIO_ODR_8 | GPIO_ODR_9 | GPIO_ODR_11 | GPIO_ODR_14);
	// sets the AFR values for PB3,4,5 as 0000 to set them as alternative functions for SPI1
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFR3 | GPIO_AFRL_AFR4 | GPIO_AFRL_AFR5);

	// enable RCC clock to GPIO A Ports
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    //clear MODER for PA0
    GPIOA->MODER &= ~GPIO_MODER_MODER0;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;

	// full duplex SPI peripheral
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // enables RCC clock to SPI1
	// spi1_cr1 config
	SPI1->CR1 &= ~(SPI_CR1_SPE);// disable spi enable pin so that spi can be configured
	SPI1->CR1 |= SPI_CR1_MSTR;  // configure spi for master mode
	SPI1->CR1 &= ~(SPI_CR1_BR); // baud rate set to fpclk/256 (slowest)
	SPI1->CR1 |= 0x38;
	SPI1->CR1 |= (SPI_CR1_SSI | SPI_CR1_SSM); // set SSM(software slave management)/SSI(internal slave select) for SPI1
	SPI1->CR2 &= ~(SPI_CR2_DS); // clears what was set for word size
	SPI1->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2); // sets word size to 8 bits
	SPI1->CR2 |= SPI_CR2_FRXTH; // set RXFIFO threshold to 8 bits
	SPI1->CR1 |= SPI_CR1_SPE;   // enable spi1
}

// function to set SPI baud rate to fpclk/16
void spi1_fast()
{
	SPI1->CR1 &= ~(SPI_CR1_SPE);// disable SPI enable pin so that SPI can be configured
	SPI1->CR1 &= ~(SPI_CR1_BR); // baud rate set to fpclk/16 (~3.2 MHz)
	SPI1->CR1 |= 0x10;
	SPI1->CR1 |= SPI_CR1_SPE;   // enable spi1
}

// taken from A.6.2 (pg. 944)
void setup_t_irq(void) {
	/*
    // enable the SYSCFGCOMP for EXTI interrupts
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	// Select Port A for pin 0 external interrupt by writing 0000 in EXTI0
	SYSCFG->EXTICR[0] &= (uint16_t)~SYSCFG_EXTICR1_EXTI0;
	// Configure corresponding mask bit in the EXTI_IMR register (EXTI_IMR_MR0 = 0x0001)
	// Writing 1 to corresponding mask bit in EXTI_IMR means "Interrupt request from Line x(0) is not masked"
	// PA0 is on line 0 for EXTI0 (figure 25 on pg. 222)
	EXTI->IMR |= EXTI_IMR_MR0;
	// Configure the Trigger Selection bits of the Interrupt line on falling edge (EXTI_FTSR_TR0 = 0x0001)
	// Writing 1 to corresponding mask bit in EXTI_FTSR means "Falling trigger enabled (for Event and Interrupt) for input line"
	EXTI->FTSR |= EXTI_FTSR_TR0;
	EXTI->RTSR &= ~EXTI_RTSR_TR0;
	// Configure NVIC for External Interrupt
	// Enable Interrupt on EXTI0_1
	NVIC->ISER[0] |= 1 << EXTI0_1_IRQn;
	// Set priority for EXTI0_1
	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	*/
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	SYSCFG->EXTICR[0] &= (uint16_t)~SYSCFG_EXTICR1_EXTI0;
	EXTI->IMR |= EXTI_IMR_MR0;
	EXTI->FTSR |= EXTI_FTSR_TR0;
	NVIC->ISER[0] |= 1 << EXTI0_1_IRQn;
	NVIC_SetPriority(EXTI0_1_IRQn, 1);

}


void EXTI0_1_IRQHandler (void) {

	// acknowledge the interrupt
	EXTI->PR |= EXTI_PR_PR0;
	//uint8_t temp;
	uint16_t tx, ty;
	uint16_t xc, yc;
	float xScale = 1024.0F/800;
	float yScale = 1024.0F/480;

	//
	if (touched()) {
		touchRead(&tx, &ty);
		/* Draw a circle */
		xc = (uint16_t)(tx/xScale);
		yc = (uint16_t)(ty/yScale);
		//drawCircle(xc, yc, 4, RA8875_WHITE, 1);

		if(buttonHandler(xc,yc) == 0){
			// return = 0, no GUI state change, acknowledge touch interrupt right away
		  writeReg(RA8875_INTC2, RA8875_INTC2_TP);
		}

		//canTouch = 0;

	}
	//	canTouch = 0;
	//	set_pin(GPIOA, 6, canTouch);
	//	//tim15_resetInterrupt();
	//}

}


// read RA8875 INT pin (A0)
uint8_t ra8875INT()
{
    return (GPIOA->IDR & GPIO_IDR_0);
}

// Transfers a byte over SPI. Does not control NSSP
uint8_t transByte(uint8_t d)
{
    uint8_t rec;
    // writes data to DR to transmit
    while((SPI->SR & SPI_SR_BSY) != 0)
        ;
    *((uint8_t*)&SPI->DR) = d;
    while((SPI->SR & SPI_SR_BSY) != 0)
        ;
    // read back from RXFIFO
    while((SPI->SR & SPI_SR_RXNE) == 0)
        ;
    rec = SPI->DR;
    return rec;
}

// Write command to the LCD
void LCD_WR_REG(uint8_t data)
{
    CS_LOW;
    transByte(RA8875_CMDWRITE);
    transByte(data);
    CS_HIGH;
}

// Write 8-bit data to the LCD
void LCD_WR_DATA(uint8_t data)
{
    CS_LOW;
    transByte(RA8875_DATAWRITE);
    transByte(data);
    CS_HIGH;
}

// read from an RA8875 register
uint8_t readReg(uint8_t reg)
{
	LCD_WR_REG(reg);
	return readData();
}

// reads data from the RA8875
uint8_t readData()
{
	uint8_t read;
	CS_LOW;
	transByte(RA8875_DATAREAD);
	read = transByte(0x0);
	CS_HIGH;
	return read;
}

// writes val to RA8875 reg
void  writeReg(uint8_t reg, uint8_t val) {
	// specify to RA8775 what register to write to
	LCD_WR_REG(reg);
	// writes the data to the register
	LCD_WR_DATA(val);
}

/****************************************************************************
 *                  LCD INITIALIZATION FUNCTIONS                            *
 ****************************************************************************/

void LCD_Init() {
	// initialize()
	// PLLinit()
	// size == 800x480
	writeReg(RA8875_PLLC1, (RA8875_PLLC1_PLLDIV1  + 11));
	nano_wait(1000000);	// need to delay 1 ms
	writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
	nano_wait(1000000);
	// out of PLLinit()
	writeReg(RA8875_SYSR, (RA8875_SYSR_16BPP | RA8875_SYSR_MCU8));
	// if size==800x480
	// Setting Timing Values
	uint8_t pixclk = (RA8875_PCSR_PDATL | RA8875_PCSR_2CLK);
	uint8_t hsync_start = 32;
	uint8_t hsync_pw = 96;
	uint8_t hsync_finetune = 0;
	uint8_t hsync_nondisp = 26;
	uint8_t vsync_pw = 2;
	uint16_t vsync_nondisp = 32;
	uint16_t vsync_start = 23;
	uint8_t voffset = 0;

	writeReg(RA8875_PCSR, pixclk);
	nano_wait(1000000);

	// Horizontal settings registers
	writeReg(RA8875_HDWR, (LCD_W / 8) - 1);
	writeReg(RA8875_HNDFTR, (RA8875_HNDFTR_DE_HIGH + hsync_finetune));
	writeReg(RA8875_HNDR, ((hsync_nondisp - hsync_finetune - 2) / 8));
	writeReg(RA8875_HSTR, ((hsync_start / 8) - 1));
	writeReg(RA8875_HPWR, (RA8875_HPWR_LOW + (hsync_pw / 8 - 1)));

	// Vertical settings registers
	writeReg(RA8875_VDHR0, (uint16_t)(LCD_H - 1 + voffset) & 0xFF);
	writeReg(RA8875_VDHR1, (uint16_t)(LCD_H - 1 + voffset) >> 8);
	writeReg(RA8875_VNDR0, vsync_nondisp - 1);
	writeReg(RA8875_VNDR1, vsync_nondisp >> 8);
	writeReg(RA8875_VSTR0, vsync_start - 1);
	writeReg(RA8875_VSTR1, vsync_start >> 8);
	writeReg(RA8875_VPWR, (RA8875_VPWR_LOW + vsync_pw - 1));

	// Set active window X
	writeReg(RA8875_HSAW0, 0);
	writeReg(RA8875_HSAW1, 0);
	writeReg(RA8875_HEAW0, (uint16_t)(LCD_W - 1) & 0xFF);
	writeReg(RA8875_HEAW1, (uint16_t)(LCD_W - 1) >> 8);

	// Set active window Y
	writeReg(RA8875_VSAW0, 0 + voffset);
	writeReg(RA8875_VSAW1, 0 + voffset);
	writeReg(RA8875_VEAW0, (uint16_t)(LCD_H - 1 + voffset) & 0xFF);
	writeReg(RA8875_VEAW1, (uint16_t)(LCD_H - 1 + voffset) >> 8);

	writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
	nano_wait(500000000);

	// set SPI speed back to fast
	spi1_fast();
}

void displayOn(int on) {
  if (on)
    writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
  else
    writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

void GPIOX(int on){
 if (on)
    writeReg(RA8875_GPIOX, 1);
  else
    writeReg(RA8875_GPIOX, 0);
}

void PWM1out(uint8_t p){
    writeReg(RA8875_P1DCR, p);
}

void PWM1config(int on, uint8_t clock) {
  if (on) {
    writeReg(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
  } else {
    writeReg(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
  }
}

uint8_t waitPoll(uint8_t regname, uint8_t waitflag) {
  /* Wait for the command to finish */
  while (1) {
    uint8_t temp = readReg(regname);
    if (!(temp & waitflag))
      return 1;
  }
  return 0; // MEMEFIX: yeah i know, unreached! - add timeout?
}

/****************************************************************************
 *                          DRAWING FUNCTIONS                               *
 ****************************************************************************/

uint16_t applyRotationX(uint16_t x) {
    return x;
}

uint16_t applyRotationY(uint16_t y) {
    return y;
}

void graphicsMode(void) {
  LCD_WR_REG(RA8875_MWCR0);
  uint8_t temp = readData();
  temp &= ~RA8875_MWCR0_TXTMODE; // bit #7
  LCD_WR_DATA(temp);
}

void fillScreen(uint16_t color) {
	drawRect(0,0,LCD_W-1,LCD_H-1,color,1);
}

void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int filled) {
	x = applyRotationX(x);
	y = applyRotationY(y);
	w = applyRotationX(w);
	h = applyRotationY(h);

	// Set X0 (sends in 2 8-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_X0_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(x & 0xff);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_X0_8);
	// sends the bigger half of the coord second
	LCD_WR_DATA(x >> 8);

	// Set Y0 (sends in 2 8-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_Y0_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(y & 0xff);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_Y0_8);
	// sends the bigger half of the coord second
	LCD_WR_DATA(y >> 8);

	// Set X1 (sends in 2 8-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_X1_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(w & 0xff);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_X1_8);
	// sends the bigger half of the coord second
	LCD_WR_DATA((w) >> 8);

	// Set Y1 (sends in 2 8-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_Y1_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(h & 0xff);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_Y1_8);
	// sends the bigger half of the coord second
	LCD_WR_DATA((h) >> 8);

	// Set Color (sends in 3 5-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_COL_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(color & 0x001f);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_COL_5);
	// sends the bigger half of the coord second
	LCD_WR_DATA((color & 0x07e0) >> 5);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_COL_11);
	// sends the bigger half of the coord second
	LCD_WR_DATA((color & 0xf800) >> 11);

	// Display on screen
	LCD_WR_REG(RA8875_DCR);
	if (filled) {
		LCD_WR_DATA(0xB0);
	} else {
		LCD_WR_DATA(0x90);
	}

	// need to wait for command to finish
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color, uint8_t filled) {
  x = applyRotationX(x);
  y = applyRotationY(y);

  /* Set X */
  LCD_WR_REG(0x99);
  LCD_WR_DATA(x);
  LCD_WR_REG(0x9a);
  LCD_WR_DATA(x >> 8);

  /* Set Y */
  LCD_WR_REG(0x9b);
  LCD_WR_DATA(y);
  LCD_WR_REG(0x9c);
  LCD_WR_DATA(y >> 8);

  /* Set Radius */
  LCD_WR_REG(0x9d);
  LCD_WR_DATA(r);

  /* Set Color */
  LCD_WR_REG(0x63);
  LCD_WR_DATA((color & 0xf800) >> 11);
  LCD_WR_REG(0x64);
  LCD_WR_DATA((color & 0x07e0) >> 5);
  LCD_WR_REG(0x65);
  LCD_WR_DATA((color & 0x001f));

  /* Draw! */
  LCD_WR_REG(RA8875_DCR);
  if (filled) {
      LCD_WR_DATA(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL);
  } else {
      LCD_WR_DATA(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
}



/****************************************************************************
 *                            TOUCH FUNCTIONS                               *
 ****************************************************************************/
void touchEnable(uint8_t on) {
  uint8_t adcClk = (uint8_t)RA8875_TPCR0_ADCCLK_DIV16;

  if (on) {
    /* Enable Touch Panel (Reg 0x70) */
    writeReg(RA8875_TPCR0, RA8875_TPCR0_ENABLE | RA8875_TPCR0_WAIT_4096CLK |
             RA8875_TPCR0_WAKEENABLE | adcClk); // 10mhz max!
    /* Set Auto Mode      (Reg 0x71) */
    writeReg(RA8875_TPCR1, RA8875_TPCR1_AUTO |
          // RA8875_TPCR1_VREFEXT |
             RA8875_TPCR1_DEBOUNCE);
    /* Enable TP INT */
    writeReg(RA8875_INTC1, readReg(RA8875_INTC1) | RA8875_INTC1_TP);
  } else {
    /* Disable TP INT */
    writeReg(RA8875_INTC1, readReg(RA8875_INTC1) & ~RA8875_INTC1_TP);
    /* Disable Touch Panel (Reg 0x70) */
    writeReg(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
  }
}

uint8_t touched(void) {
  if (readReg(RA8875_INTC2) & RA8875_INTC2_TP)
    return 1;
  return 0;
}

/*********************************************/
/*!
      Reads the last touch event
      @param x  Pointer to the uint16_t field to assign the raw X value
      @param y  Pointer to the uint16_t field to assign the raw Y value
      @return True if successful
      @note Calling this function will clear the touch panel interrupt on
            the RA8875, resetting the flag used by the 'touched' function
*/
/*********************************************/
uint8_t touchRead(uint16_t *x, uint16_t *y) {
  uint16_t tx, ty;
  uint8_t temp;

  tx = readReg(RA8875_TPXH);
  ty = readReg(RA8875_TPYH);
  temp = readReg(RA8875_TPXYL);
  tx <<= 2;
  ty <<= 2;
  tx |= temp & 0x03;        // get the bottom x bits
  ty |= (temp >> 2) & 0x03; // get the bottom y bits

  *x = tx;
  *y = ty;

  /* Clear TP INT Status */
  //writeReg(RA8875_INTC2, RA8875_INTC2_TP);

  return 1;
}

/**************************************************************************
 *                              TEXT FUNCTIONS                            *
 **************************************************************************/
void textMode() {
  /* Set text mode */
  LCD_WR_REG(RA8875_MWCR0);
  uint8_t temp = readData();
  temp |= RA8875_MWCR0_TXTMODE; // Set bit 7
  LCD_WR_DATA(temp);

  /* Select the internal (ROM) font */
  LCD_WR_REG(0x21);
  temp = readData();
  temp &= ~((1 << 7) | (1 << 5)); // Clear bits 7 and 5
  LCD_WR_DATA(temp);
}

/****************************************/
/*!
     Enable Cursor Visibility and Blink
     Here we set bits 6 and 5 in 40h
     As well as the set the blink rate in 44h
     The rate is 0 through max 255
     the lower the number the faster it blinks (00h is 1 frame time,
     FFh is 256 Frames time.
     Blink Time (sec) = BTCR[44h]x(1/Frame_rate)
     @param rate The frame rate to blink
 */
/****************************************/

void cursorBlink(uint8_t rate) {

  LCD_WR_REG(RA8875_MWCR0);
  uint8_t temp = readData();
  temp |= RA8875_MWCR0_CURSOR;
  LCD_WR_DATA(temp);

  LCD_WR_REG(RA8875_MWCR0);
  temp = readData();
  temp |= RA8875_MWCR0_BLINK;
  LCD_WR_DATA(temp);

  if (rate > 255)
    rate = 255;
  LCD_WR_REG(RA8875_BTCR);
  LCD_WR_DATA(rate);
}

/****************************************************************/
/*!
      Sets the display in text mode (as opposed to graphics mode)
      @param x The x position of the cursor (in pixels, 0..1023)
      @param y The y position of the cursor (in pixels, 0..511)
*/
/****************************************************************/
void textSetCursor(uint16_t x, uint16_t y) {
  x = applyRotationX(x);
  y = applyRotationY(y);

  /* Set cursor location */
  LCD_WR_REG(0x2A);
  LCD_WR_DATA(x & 0xFF);
  LCD_WR_REG(0x2B);
  LCD_WR_DATA(x >> 8);
  LCD_WR_REG(0x2C);
  LCD_WR_DATA(y & 0xFF);
  LCD_WR_REG(0x2D);
  LCD_WR_DATA(y >> 8);
}

/******************************************************************/
/*!
      Sets the fore color when rendering text with a transparent bg
      @param foreColor The RGB565 color to use when rendering the text
*/
/******************************************************************/
void textTransparent(uint16_t foreColor) {
  /* Set Fore Color */
  LCD_WR_REG(0x63);
  LCD_WR_DATA((foreColor & 0xf800) >> 11);
  LCD_WR_REG(0x64);
  LCD_WR_DATA((foreColor & 0x07e0) >> 5);
  LCD_WR_REG(0x65);
  LCD_WR_DATA((foreColor & 0x001f));

  /* Set transparency flag */
  LCD_WR_REG(0x22);
  uint8_t temp = readData();
  temp |= (1 << 6); // Set bit 6
  LCD_WR_DATA(temp);
}

/******************************************************/
/*!
      Renders some text on the screen when in text mode
      @param buffer    The buffer containing the characters to render
      @param len       The size of the buffer in bytes
*/
/******************************************************/
void textWrite(const char *buffer, uint16_t len) {

  LCD_WR_REG(RA8875_MRWC);
  for (uint16_t i = 0; i < len; i++) {
    LCD_WR_DATA(buffer[i]);
    if (_textScale > 0)
      nano_wait(500000);
  }
}

/***********************************************************/
/*!
      Sets the fore and background color when rendering text
      @param foreColor The RGB565 color to use when rendering the text
      @param bgColor   The RGB565 colot to use for the background
*/
/***********************************************************/
void textColor(uint16_t foreColor, uint16_t bgColor) {
  /* Set Fore Color */
  LCD_WR_REG(0x63);
  LCD_WR_DATA((foreColor & 0xf800) >> 11);
  LCD_WR_REG(0x64);
  LCD_WR_DATA((foreColor & 0x07e0) >> 5);
  LCD_WR_REG(0x65);
  LCD_WR_DATA((foreColor & 0x001f));

  /* Set Background Color */
  LCD_WR_REG(0x60);
  LCD_WR_DATA((bgColor & 0xf800) >> 11);
  LCD_WR_REG(0x61);
  LCD_WR_DATA((bgColor & 0x07e0) >> 5);
  LCD_WR_REG(0x62);
  LCD_WR_DATA((bgColor & 0x001f));

  /* Clear transparency flag */
  LCD_WR_REG(0x22);
  uint8_t temp = readData();
  temp &= ~(1 << 6); // Clear bit 6
  LCD_WR_DATA(temp);
}

/*****************************************************************/
/*!
      Sets the text enlarge settings, using one of the following values:
      0 = 1x zoom
      1 = 2x zoom
      2 = 3x zoom
      3 = 4x zoom
      @param scale   The zoom factor (0..3 for 1-4x zoom)
*/
/*****************************************************************/
void textEnlarge(uint8_t scale) {
  if (scale > 3)
    scale = 3; // highest setting is 3

  /* Set font size flags */
  LCD_WR_REG(0x22);
  uint8_t temp = readData();
  temp &= ~(0xF); // Clears bits 0..3
  temp |= scale << 2;
  temp |= scale;

  LCD_WR_DATA(temp);

  _textScale = scale;
}
