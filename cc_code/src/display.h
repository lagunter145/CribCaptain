/*
 * display.h
 *
 *  Created on: Jan 24, 2023
 *      Author: Lauren
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

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


#endif /* DISPLAY_H_ */
