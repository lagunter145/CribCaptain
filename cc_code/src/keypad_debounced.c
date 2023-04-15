/**
  ******************************************************************************
  * @file    keypad_debounced.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   keypad with debouncing.
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include <stdint.h>
#include "keypad_debounced.h"
#include "keypad_support.h"
#include "RA8775_commands.h"
#include "colours.h"
#include "gui.h"
#include "timer.h"

extern stateType guiMenuState;
extern uint16_t base_color;
extern uint16_t acce_color;
extern uint16_t colour_pairs[16][2];

uint8_t c = 1;
uint8_t r = 0;

//enables GPIO ports for Keypad
void enable_ports_keypad(){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
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
     GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8 |
                 GPIO_OTYPER_OT_9 |
                 GPIO_OTYPER_OT_10 |
                 GPIO_OTYPER_OT_11);
    GPIOC->OTYPER |= GPIO_OTYPER_OT_8 |
                 GPIO_OTYPER_OT_9 |
                 GPIO_OTYPER_OT_10 |
                 GPIO_OTYPER_OT_11;
}

//set up Timer 7 for Keypad
void setup_tim7() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 4800 - 1;
    TIM7->ARR = 10 - 1; //1 ms
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= (1 << TIM7_IRQn);
    TIM7->CR1 |= TIM_CR1_CEN;
}

//Timer 7 Handler for Keypad
void TIM7_IRQHandler()
{
    //acknowledge the interrupt
    TIM7->SR &= ~TIM_SR_UIF;

    for (int c = 1; c <= 4; c++){
        drive_column(c);
        update_history(c, r);
        r = read_rows();
    }
    /*switch(guiMenuState) {
    	case LOADING:
    		guiLOADINGDraw();
    		break;
    	case MAIN:
    		guiMAINDraw();
    		break;
    }*/
}


//This function uses the button press "val"  to light the LEDs in hex
void keypad_values(char val){
    switch(val){
    case '1':
        base_color = colour_pairs[0][0];
        acce_color = colour_pairs[0][1];
        break;
    case '2':
    	base_color = colour_pairs[1][0];
	    acce_color = colour_pairs[1][1];
        break;
    case '3':
    	base_color = colour_pairs[2][0];
		acce_color = colour_pairs[2][1];
        break;
    case '4':
    	base_color = colour_pairs[3][0];
		acce_color = colour_pairs[3][1];
        break;
    case '5':
    	base_color = colour_pairs[4][0];
		acce_color = colour_pairs[4][1];
         break;
    case '6':
    	base_color = colour_pairs[5][0];
		acce_color = colour_pairs[5][1];
        break;
    case '7':
    	base_color = colour_pairs[6][0];
		acce_color = colour_pairs[6][1];
        break;
    case '8':
    	base_color = colour_pairs[7][0];
		acce_color = colour_pairs[7][1];
        break;
    case '9':
    	base_color = colour_pairs[8][0];
		acce_color = colour_pairs[8][1];
        break;
    case 'A':
    	base_color = colour_pairs[9][0];
		acce_color = colour_pairs[9][1];
        break;
    case 'B':
    	base_color = colour_pairs[10][0];
		acce_color = colour_pairs[10][1];
        break;
    case 'C':
    	base_color = colour_pairs[11][0];
		acce_color = colour_pairs[11][1];
        break;
    case 'D':
    	base_color = colour_pairs[12][0];
		acce_color = colour_pairs[12][1];
        break;
    case '*':
    	base_color = colour_pairs[13][0];
		acce_color = colour_pairs[13][1];
        break;
    case '#':
    	base_color = colour_pairs[14][0];
		acce_color = colour_pairs[14][1];
        break;
    case '0':
    	base_color = colour_pairs[15][0];
		acce_color = colour_pairs[15][1];
        break;
    default:
        break;
    }
}
