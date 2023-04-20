
//===========================================================================
// ECE 362 lab experiment 10 -- Asynchronous Serial Communication
//===========================================================================

#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "fifo.h"
#include "tty.h"
#include <string.h> // for memset()
#include <stdio.h> // for printf()

void advance_fattime(void);
void command_shell(void);

// Write your subroutines below.
void setup_usart5()
{
    // Enable the RCC clocks to GPIOC and GPIOD
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC -> AHBENR |= RCC_AHBENR_GPIODEN;

    // Configure pin PC12 to be routed to USART5_TX (AF2)
    GPIOC -> MODER &= ~GPIO_MODER_MODER12;
    GPIOC -> MODER |= GPIO_MODER_MODER12_1; //alt func
    GPIOC -> AFR[1] &= ~GPIO_AFRH_AFR12;
    GPIOC -> AFR[1] |= 1 << (4 * (12-8) + 1); //set AF2

    // Configure pin PD2 to be routed to USART5_RX (AF2)
    GPIOD -> MODER &= ~GPIO_MODER_MODER2;
    GPIOD -> MODER |= GPIO_MODER_MODER2_1;
    GPIOD -> AFR[0] &= ~GPIO_AFRL_AFR2;
    GPIOD -> AFR[0] |= 1 << (4 * 2 + 1); //set AF2

    // Enable the RCC clock to the USART5 peripheral
    RCC -> APB1ENR |= RCC_APB1ENR_USART5EN;

    // Configure USART5
    USART5 -> CR1 &= ~USART_CR1_UE; // Disable it before modifying
    USART5 -> CR1 &= ~USART_CR1_M; // Set word size to 8 bits (M[1:0] = 00 for 8 data bits)
    USART5 -> CR1 &= ~(1 << 28); //??
    USART5 -> CR2 &= ~USART_CR2_STOP; // Set it for one stop bit     (00) = 1 stop bit (FRM 727)
    USART5 -> CR1 &= ~USART_CR1_PCE; // Set it for no parity
    USART5 -> CR1 &= ~USART_CR1_OVER8; // Use 16x oversampling
    USART5 -> BRR = 417; // Use a baud rate of 115200 (115.2 kbaud)
    USART5 -> CR1 |= USART_CR1_TE; // Enable the transmitter and the receiver by setting the TE and RE bits
    USART5 -> CR1 |= USART_CR1_RE;
    USART5 -> CR1 |= USART_CR1_UE; // Enable the USART

    // Wait for the TE and RE bits to be acknowledged. This indicates that the USART is ready to transmit and receive
    while (((USART5 -> ISR & USART_ISR_TEACK) == 0) || ((USART5 -> ISR & USART_ISR_REACK) == 0));
    //while (((USART5 -> ISR) & USART_ISR_RXNE == 0) && ((USART5 -> ISR) & USART_ISR_TXE == 0));
        // ??

}

int simple_putchar(int n)
{
    while ((USART5 -> ISR & USART_ISR_TXE) == 0); // Wait for the USART5 ISR TXE to be set
    USART5 -> TDR = n; // Write the argument to the USART5 TDR (transmit data register)
    return n;
}

int simple_getchar()
{
    while ((USART5 -> ISR & USART_ISR_RXNE) == 0); // Wait for the USART5 ISR RXNE to be set
    return USART5 -> RDR;
}

int better_putchar(int n)
{
    if (n == '\n') {
        while ((USART5 -> ISR & USART_ISR_TXE) == 0); // Wait for the USART5 ISR TXE to be set
        USART5 -> TDR = '\r';
    }

    while ((USART5 -> ISR & USART_ISR_TXE) == 0); // Wait for the USART5 ISR TXE to be set
    USART5 -> TDR = n; // Write the argument to the USART5 TDR (transmit data register)
    return n;
}

int better_getchar()
{
    while ((USART5 -> ISR & USART_ISR_RXNE) == 0); // Wait for the USART5 ISR RXNE to be set
    if (USART5 -> RDR == '\r') {
        return '\n';
    } else {
        return USART5 -> RDR;
    }
}

int interrupt_getchar(void) {
    while (fifo_newline(&input_fifo) == 0) {
        asm volatile("wfi"); //wait for an interrupt
    }

    // Remove the first character from fifo and return it
    char ch = fifo_remove(&input_fifo);
    return ch;
}

int __io_putchar(int ch) {
    return better_putchar(ch);
}

int __io_getchar(void) {
    return interrupt_getchar();
    //return line_buffer_getchar();
    //return better_getchar();
}



void USART3_4_5_6_7_8_IRQHandler()
{
    if (USART5->ISR & USART_ISR_ORE) // Check and clear the ORE flag
        USART5->ICR |= USART_ICR_ORECF;

    // Read the new character from the USART5 RDR
    char ch = USART5 -> RDR;

    // Check if the input_fifo is full. If it is, return from the ISR. (Throw away the character.)
    if (fifo_full(&input_fifo)) {
        return;
    }

    // Call insert_echo_char() with the character
    insert_echo_char(ch);
}

void enable_tty_interrupt()
{
    // Configures USART5 to raise an interrupt every time the receive data register becomes not empty bc a new character has been received
    USART5 -> CR1 |= USART_CR1_RXNEIE;

    // Enable NVIC
    NVIC -> ISER[0] |= 1<<USART3_8_IRQn; //enable the interrupt to be accepted by NVIC ISER
}



void setup_spi1()
{
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN; // Enable the RCC clock to GPIOA.
    GPIOA -> MODER &= ~GPIO_MODER_MODER12; // Configure PA12 to be a general-purpose output.
    GPIOA -> MODER |= GPIO_MODER_MODER12_0;
    GPIOA -> MODER &= ~GPIO_MODER_MODER5; // Configure GPIOA so that pins 5, 6, and 7 are routed to SPI1. DO NOT USE PA4 FOR ANYTHING. IT IS NOT BEING USED AS NSS.
    GPIOA -> MODER |= GPIO_MODER_MODER5_1;
    GPIOA -> AFR[0] &= ~GPIO_AFRL_AFR5; //AF0 = SPI1_SCK
    GPIOA -> MODER &= ~GPIO_MODER_MODER6;
    GPIOA -> MODER |= GPIO_MODER_MODER6_1;
    GPIOA -> AFR[0] &= ~GPIO_AFRL_AFR6; //AF0 = SPI1_MISO
    GPIOA -> MODER &= ~GPIO_MODER_MODER7;
    GPIOA -> MODER |= GPIO_MODER_MODER7_1;
    GPIOA -> AFR[0] &= ~GPIO_AFRL_AFR7; //AF0 = SPI1_MOSI

    RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable the RCC clock to the SPI1 peripheral.
    SPI1 -> CR1 &= ~SPI_CR1_SPE; // Disable the SPI1 peripheral by turning off the SPE bit
    SPI1 -> CR1 |= SPI_CR1_BR; // Set baud rate as low as possible (48MHz / 256)
    SPI1 -> CR1 &= ~SPI_CR1_BIDIMODE; // Ensure that BIDIMODE and BIDIOE are cleared.
    SPI1 -> CR1 &= ~SPI_CR1_BIDIOE;
    SPI1 -> CR1 |= SPI_CR1_MSTR; // Enable Master mode
    SPI1 -> CR2 |= SPI_CR2_NSSP; // Set NSSP and configure the peripheral for an 8-bit word (which is the default).
    SPI1 -> CR2 &= ~SPI_CR2_DS; // (defaults to 8-bit 0111)
    SPI1 -> CR2 |= SPI_CR2_FRXTH; // Set the bit that sets the FIFO-reception threshold to 8-bits.
    SPI1 -> CR1 |= SPI_CR1_SPE; // Enable the SPI channel
}

void spi_high_speed()
{
    SPI1 -> CR1 &= ~SPI_CR1_SPE; // Disable the SPI1 peripheral by turning off the SPE bit
    //SPI1 -> CR1 &= ~SPI_CR1_BR; // Configure SPI1 for a 24 MHz SCK rate

    SPI1 -> CR1 &= ~SPI_CR1_BR; // Configure SPI1 for a 12 MHz SCK rate
    SPI1 -> CR1 |= SPI_CR1_BR_1;
    SPI1 -> CR1 |= SPI_CR1_SPE; // Enable the SPI channel
}

void TIM14_IRQHandler()
{
    TIM14 -> SR &= ~TIM_SR_UIF; //Acknowledge
    advance_fattime();
}

void setup_tim14()
{
    RCC -> APB1ENR |= RCC_APB1ENR_TIM14EN; //enable clock

    TIM14 -> CR1 &= ~TIM_CR1_CEN; // disable

    TIM14 -> PSC = 60000-1;
    TIM14 -> ARR = 1600-1; //0.5 Hz

    TIM14 -> DIER |= TIM_DIER_UIE; //enable interrupt on update
    TIM14 -> CR1 |= TIM_CR1_CEN; // enable

    NVIC -> ISER[0] |= 1<<TIM14_IRQn; //enable the interrupt to be accepted by NVIC ISER
}

// Write your subroutines above.

const char testline[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\r\n";

int main()
{
    setup_usart5();

    // Uncomment these when you're asked to...
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    // Test for 2.2 simple_putchar()
//
//    for(;;)
//        for(const char *t=testline; *t; t++)
//            simple_putchar(*t);

    // Test for 2.3 simple_getchar()
//
//    for(;;)
//        simple_putchar( simple_getchar() );

    // Test for 2.4 and 2.5 __io_putchar() and __io_getchar()
//
//    printf("Hello!\n");
//    for(;;)
//        putchar( getchar() );

    // Test 2.6
//
//    for(;;) {
//        printf("Enter string: ");
//        char line[100];
//        fgets(line, 99, stdin);
//        line[99] = '\0'; // just in case
//        printf("You entered: %s", line);
//    }


    // Test for 2.7
//
//    enable_tty_interrupt();
//    for(;;) {
//        printf("Enter string: ");
//        char line[100];
//        fgets(line, 99, stdin);
//        line[99] = '\0'; // just in case
//        printf("You entered: %s", line);
//    }

    // Test for 2.8 Test the command shell and clock

    //enable_tty_interrupt();
    //setup_tim14();
    FATFS fs_storage;
    FATFS *fs = &fs_storage;
    f_mount(fs, "", 1);
    command_shell();

    return 0;
}
