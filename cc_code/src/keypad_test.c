/*
 * keypad_test.c
 *
 *  Created on: Apr 14, 2023
 *      Author: gunter3
 */


/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "keypad_test.h"

#include "colors.h"

extern volatile uint16_t base_color;
extern volatile uint16_t acce_color;
extern volatile uint16_t color_pairs[16][2];
extern volatile uint8_t colorUpdated;

//enables GPIO ports for Keypad and LEDs
void enable_ports_keypad_LED(){
    //Keypad
    //turn on clock for keypad
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    //clear PC4-PC11 for Keypad
    GPIOC->MODER &= ~(GPIO_MODER_MODER4 |
                    GPIO_MODER_MODER5 |
                    GPIO_MODER_MODER6 |
                    GPIO_MODER_MODER7 |
                    GPIO_MODER_MODER8 |
                    GPIO_MODER_MODER9 |
                    GPIO_MODER_MODER10 |
                    GPIO_MODER_MODER11);

    //PC4-PC7: rows are inputs
    //PC4-PC7 pulled internally low
     GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR4 |
                     GPIO_PUPDR_PUPDR5 |
                     GPIO_PUPDR_PUPDR6 |
                     GPIO_PUPDR_PUPDR7);
     GPIOC->PUPDR |= GPIO_PUPDR_PUPDR4_1 |
                     GPIO_PUPDR_PUPDR5_1 |
                     GPIO_PUPDR_PUPDR6_1 |
                     GPIO_PUPDR_PUPDR7_1;
     //PC8-PC11: columns are outputs
     GPIOC->MODER |= GPIO_MODER_MODER8_0 |
                 GPIO_MODER_MODER9_0 |
                 GPIO_MODER_MODER10_0 |
                 GPIO_MODER_MODER11_0;


     //LED
     //turn on clock for LEDs
      RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
      //clear PC0-PC3 for LEDs
      GPIOA->MODER &= ~(GPIO_MODER_MODER5 |
                      GPIO_MODER_MODER6 |
                      GPIO_MODER_MODER7 |
                      GPIO_MODER_MODER8);
      //set PC0-PC3 for outputs
      GPIOA->MODER |= GPIO_MODER_MODER5_0 |
                      GPIO_MODER_MODER6_0 |
                      GPIO_MODER_MODER7_0 |
                      GPIO_MODER_MODER8_0;
}

//set up Timer 7 for Keypad
void setup_tim7() {
    RCC->APB1ENR|= RCC_APB1ENR_TIM7EN; //turn on clock

    //set timer to update event exactly once per millisecond (f = 1 kHz)
    TIM7->PSC = 4800 - 1;
    TIM7->ARR = 1000 - 1;

    //Update Interrupt enable
    TIM7->DIER |= TIM_DIER_UIE;
    //enable interrupt in NVIC ISER
    NVIC->ISER[0] |= (1 << TIM7_IRQn);
	NVIC_SetPriority(TIM7_IRQn, 2);
    //enable the counter
    TIM7->CR1 |= TIM_CR1_CEN;
}

//Timer 7 Handler for Keypad
void TIM7_IRQHandler()
{
    //acknowledge the interrupt
    TIM7->SR &= ~TIM_SR_UIF;

    for (int col = 1; col <= 4; col++){

        //clear all columns and set 'col' column to 1
        drive_column(col);

        //used to slow down the checking of the pins
        mysleep();

        //Given 'col' column is asserted, what rows are also asserted
        int row_val = read_rows();
        int row4 = (row_val >> 4) & 0x1;
        int row3 = (row_val >> 5) & 0x1;
        int row2 = (row_val >> 6) & 0x1;
        int row1 = (row_val >> 7) & 0x1;

        //key to convert asserted row and column to HEX value and update the color pairs
        //If a key is detected colorUpdated will become anything other than a 0
        //this will then be recognized in guiStateHandler during the once per second
        //interrupt from the timing synchronization
        colorUpdated |=  Keypad_to_LEDs(col, row1, row2, row3, row4);
    }


}

//Read the rows values of the keypad
int read_rows() {
    //isolate rows (PC4-PC7)
    int row_vals = (GPIOC->IDR & 0xF0);
    return row_vals;
}

//Drive one column of the keypad at a time
void drive_column(int col_val) {
    //clears columns
    GPIOC->ODR &= ~ (GPIO_ODR_8 |
                     GPIO_ODR_9 |
                     GPIO_ODR_10 |
                     GPIO_ODR_11);

    //sets col_val column to be 1
    GPIOC->ODR |= (0x1 << (7 + col_val));
}

//This function sets a GPIO pin num to val
void setn(int32_t pin_num, int32_t val) {
    if(val != 0)
    {
        //if value is not zero, set GPIO pin num to 1
        GPIOA->BSRR |= (0x1 << pin_num);
    }
    else
    {
        //if value is zero, set GPIO pin num to 0
        GPIOA->BSRR |= (0x1 << (pin_num + 16));
    }
}

//This functions uses the column and row that is asserted on the keyboard
//to light the LEDs (showing the HEX equivalent of the button pressed)
int Keypad_to_LEDs(int col_val, int row1, int row2, int row3, int row4){
    //Display HEX value of keyboard input on LEDs

    uint8_t val = 0;
    if(col_val == 4){
        if(row1 == 1){
            //HEX: 1
            base_color = color_pairs[0][0];
        	acce_color = color_pairs[0][1];
            val = '1';
        }
        else if(row2 == 1){
            //HEX: 4
        	base_color = color_pairs[3][0];
        	acce_color = color_pairs[3][1];
            val = '4';

        }
        else if(row3 == 1){
            //HEX: 7
        	base_color = color_pairs[5][0];
        	acce_color = color_pairs[5][1];
            val = '7';
        }
        else if(row4 == 1)  {
            //HEX: * -- treated as E for the purpose of this test
        	base_color = color_pairs[13][0];
        	acce_color = color_pairs[13][1];
            val = '*';
        }
    }
    else if(col_val == 3){
        if(row1 == 1){
            //HEX:2
        	base_color = color_pairs[1][0];
        	acce_color = color_pairs[1][1];
            val = '2';
        }
        else if(row2 == 1){
            //HEX: 5
        	base_color = color_pairs[4][0];
        	acce_color = color_pairs[4][1];
            val = '5';
        }
        else if(row3 == 1){
            //HEX: 8
        	base_color = color_pairs[7][0];
        	acce_color = color_pairs[7][1];
            val = '8';
        }
        else if(row4 == 1){
            //HEX: 0
            val = '0';
        }
    }
    else if(col_val == 2){
        if(row1 == 1){
            //HEX: 3
        	base_color = color_pairs[2][0];
        	acce_color = color_pairs[2][1];
            val = '3';
        }
        else if(row2 == 1){
            //HEX: 6
        	base_color = color_pairs[5][0];
        	acce_color = color_pairs[5][1];
            val = '6';
        }
        else if(row3 == 1){
            //HEX: 9
        	base_color = color_pairs[8][0];
        	acce_color = color_pairs[8][1];
            val = '9';
        }
        else if(row4 == 1) {
            //HEX: # -- treated as F for purpose of this test
        	base_color = color_pairs[14][0];
        	acce_color = color_pairs[14][1];
            val = '#';
        }
    }
    else if(col_val == 1){
        if(row1 == 1){
            //HEX: A
        	base_color = color_pairs[9][0];
        	acce_color = color_pairs[9][1];
            val = 'A';
        }
        else if(row2 == 1){
            //HEX: B
        	base_color = color_pairs[10][0];
        	acce_color = color_pairs[10][1];
            val = 'B';
        }
        else if(row3 == 1){
            //HEX: C
        	base_color = color_pairs[11][0];
        	acce_color = color_pairs[11][1];
            val = 'C';
        }
        else if(row4 == 1){
            //HEX: D
            val = 'D';
            base_color = color_pairs[12][0];
            acce_color = color_pairs[12][1];
        }
    }

    return val;
}

//This function slows down the scanning of the rows
//Without this, it will scan too fast
void mysleep(void) {
    for(int n = 0; n < 1000; n++);
}



