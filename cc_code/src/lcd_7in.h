/*
 * lcd_7in.h
 *
 *  Created on: Feb 8, 2023
 *      Author: gunter3
 */

#ifndef LCD_7IN_H_
#define LCD_7IN_H_

#define SPI SPI1

void nano_wait(unsigned int);

// display's chip select: PB8
#define CS_NUM  8
#define CS_BIT  (1<<CS_NUM)
#define CS_HIGH do { GPIOB->BSRR = GPIO_BSRR_BS_8; } while(0)
#define CS_LOW do { GPIOB->BSRR = GPIO_BSRR_BR_8; } while(0)
// display's touch feature chip select: PB9
#define CST_NUM  9
#define CST_BIT  (1<<CST_NUM)
#define CST_HIGH do { GPIOB->BSRR = GPIO_BSRR_BS_9; } while(0)
#define CST_LOW do { GPIOB->BSRR = GPIO_BSRR_BR_9; } while(0)
// display's reset: PB11
#define RESET_NUM 11
#define RESET_BIT (1<<RESET_NUM)
#define RESET_HIGH do { GPIOB->BSRR = GPIO_BSRR_BS_11; } while(0)
#define RESET_LOW  do { GPIOB->BSRR = GPIO_BSRR_BR_11; } while(0)
// display's DC: PB14
#define DC_NUM 14
#define DC_BIT (1<<DC_NUM)
#define DC_HIGH do { GPIOB->BSRR = GPIO_BSRR_BS_14; } while(0)
#define DC_LOW  do { GPIOB->BSRR = GPIO_BSRR_BR_14; } while(0)
// display's SCK
#define SCK_NUM 3
#define SCK_BIT (1<<SCK_NUM)
#define SCK_HIGH do { GPIOB->BSRR = GPIO_BSRR_BS_3; } while(0)
#define SCK_LOW  do { GPIOB->BSRR = GPIO_BSRR_BR_3; } while(0)
// display's SCK
#define CLK_NUM 3
#define CLK_BIT (1<<SCK_NUM)
#define CLK_HIGH do { GPIOB->BSRR = GPIO_BSRR_BS_3; } while(0)
#define CLK_LOW  do { GPIOB->BSRR = GPIO_BSRR_BR_3; } while(0)
// touch's DIN
#define DIN_NUM 5
#define DIN_BIT (1<<DIN_NUM)
#define DIN_HIGH do { GPIOB->BSRR = GPIO_BSRR_BS_5; } while(0)
#define DIN_LOW  do { GPIOB->BSRR = GPIO_BSRR_BR_5; } while(0)
// touch's DIN
#define DIN_NUM 5
#define DIN_BIT (1<<DIN_NUM)
#define DIN_HIGH do { GPIOB->BSRR = GPIO_BSRR_BS_5; } while(0)
#define DIN_LOW  do { GPIOB->BSRR = GPIO_BSRR_BR_5; } while(0)

#define LCD_W 800
#define LCD_H 480

// RA9885-specific commands
#define DISPLAY    0x90
#define FILLED     0xB0
// RA8775's commands to set initial xcoord, sent in 2 packets of 8-bit messages
#define SET_X0_0   0x91
#define SET_X0_8   0x92
// RA8775's commands to set initial ycoord, sent in 2 packets of 8-bit messages
#define SET_Y0_0   0x93
#define SET_Y0_8   0x94
// RA8775's commands to set final xcoord, sent in 2 packets of 8-bit messages
#define SET_X1_0   0x95
#define SET_X1_8   0x96
// RA8775's commands to set final ycoord, sent in 2 packets of 8-bit messages
#define SET_Y1_0   0x97
#define SET_Y1_8   0x98
// RA8775's commands to set color, sent in 3 packets of 5-bit messages
#define SET_COL_0  0x65
#define SET_COL_5  0x64
#define SET_COL_11 0x63

void setup_spi1();
static void tft_select(int val);
void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int filled);

uint16_t applyRotationX(uint16_t x);
uint16_t applyRotationY(uint16_t y);

#endif /* LCD_7IN_H_ */
