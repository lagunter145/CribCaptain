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

//enable ports for LEDs
void enable_ports_LED(){
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
    RCC->APB1ENR|= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 4800 - 1;
    TIM7->ARR = 10 - 1;
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

//This function uses the button press "val"  to light the LEDs in hex
void keypad_values(char val){
    switch(val){
    case '1':
        setn(5, 1);
        break;
    case '2':
        setn(6, 1);
        break;
    case '3':
        setn(5, 1);
        setn(6, 1);
        break;
    case '4':
        setn(7, 1);
        break;
    case '5':
         setn(5, 1);
         setn(7, 1);
         break;
    case '6':
        setn(6, 1);
        setn(7, 1);
        break;
    case '7':
        setn(5, 1);
        setn(6, 1);
        setn(7, 1);
        break;
    case '8':
        setn(8, 1);
        break;
    case '9':
        setn(5, 1);
        setn(8, 1);
        break;
    case 'A':
        setn(6, 1);
        setn(8, 1);
        break;
    case 'B':
        setn(5, 1);
        setn(6, 1);
        setn(8, 1);
        break;
    case 'C':
        setn(8, 1);
        setn(7, 1);
        break;
    case 'D':
        setn(5, 1);
        setn(7, 1);
        setn(8, 1);
        break;
    case '*':
        setn(6, 1);
        setn(7, 1);
        setn(8, 1);
        break;
    case '#':
        setn(5, 1);
        setn(6, 1);
        setn(7, 1);
        setn(8, 1);
        break;
    case '0':
        break;
    default:
        break;
    }
}
