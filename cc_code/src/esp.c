/*
 * esp.c
 *
 *  Created on: Feb 3, 2023
 *      Author: mrahf
 */


#include "stm32f0xx.h"
#include "esp.h"
#include <stdint.h>
#include <string.h>
#include "lcd_7in.h"
#include <stdlib.h>
#include "timer.h"

char readBuffer[10];

extern volatile int minute;
extern volatile int hour;
extern volatile int second;

extern volatile int wifiHTTPState;
extern volatile int wifiInitialState;
extern volatile int tim6semaphore;


void setup_uart1() {
	//Enable RCC clocks to GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//Configure PA9  as USART1_TX
	GPIOA->MODER &= ~GPIO_MODER_MODER9;
	GPIOA->MODER |= GPIO_MODER_MODER9_1;
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFR9;
	GPIOA->AFR[1] |= 0x00000010; //Alternate function 1
	//Configure PA10 as USART1_RX
	GPIOA->MODER &= ~GPIO_MODER_MODER10;
	GPIOA->MODER |= GPIO_MODER_MODER10_1;
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFR10;
	GPIOA->AFR[1] |= 0x00000100; //Alternate function 1
	//Configure PA11 as RESET
	GPIOA->MODER &= ~GPIO_MODER_MODER11;
	GPIOA->MODER |= GPIO_MODER_MODER11_0;  //output
	GPIOA->BSRR |= GPIO_BSRR_BS_11; //set the reset high
	//Enable the RCC clock to the USART1 peripheral
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	//disable USART1
	USART1->CR1 &= ~USART_CR1_UE;
	//Set a Word Size of 8 bits
	//USART1->CR1 &= ~(USART_CR1_M | 0x10000000);
	USART1->CR1 &= ~USART_CR1_M;
	USART1->CR1 &= ~(1<<28);
	//Set it for one stop bit
	USART1->CR2 &= ~USART_CR2_STOP;
	//Set it for no parity
	USART1->CR1 &= ~USART_CR1_PCE;
	//use 16x oversampling
	USART1->CR1 &= ~USART_CR1_OVER8;

	//Not Working
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC->ISER[0] = (1 << USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 0);

	//Enable the recieve not empty interrupt enable
	USART1->CR1 |= USART_CR1_RXNEIE;

	//Set the peripheral register address in DMA_CPARx
	//DMA1->CPAR[0] |= USART1;
	//Enable DMA reciever access
	//USART1->CR3 |= USART_CR3_DMAR;

	//Baud rate of 115200
	USART1->BRR = 0;
	USART1->BRR |= 0x1A1;
	//enable the USART1
	USART1->CR1 |= USART_CR1_UE;
	//Enable the transmitter and receiver
	USART1->CR1 |= USART_CR1_RE | USART_CR1_TE;

	//Wait for the TE and RE bits to be acknowledged
	while(!((USART1->ISR & USART_ISR_TEACK) && (USART1->ISR & USART_ISR_REACK)));

	//Enable the DMA clock
	//RCC->AHBENR |= RCC_AHBENR_DMAEN;

}

//The WI-FI will be reset when PA11 is set low
void wifi_reset() {
	GPIOA->BSRR |= GPIO_BSRR_BR_11; //sets the reset pin low
	GPIOA->BSRR |= GPIO_BSRR_BS_11; //sets the reset pin high
}

uint8_t wifi_sendchar(uint8_t txChar) {
	//Wait for the USART5 ISR TXE (checks if the transmit data register is empty) to be set
	while(!(USART1->ISR & USART_ISR_TXE));
	//write the argunment to the USART TDR (transmit data register)
	USART1->TDR = txChar;

	return txChar;
}

uint8_t wifi_getchar(void) {
	//Wait for the USART5 ISR RXNE (read data register is not empty) bit to be set
	//USART1->ICR |= USART_ICR_FECF;
	while(!(USART1->ISR & USART_ISR_RXNE));
	//return the value of the receive data register

	uint8_t c = USART1->RDR;
	return c;
}

char * wifi_sendstring(char * cmd) {
	//loops through the string and sends individual characters
	for (int i = 0; i < strlen(cmd); i++) {
		wifi_sendchar(cmd[i]);
	}
	return cmd;
}


char wifi_checkstring(char * response) {
	char buf[11];
	for (int i = 0; i < strlen(response); i++) {
		uint8_t character = wifi_getchar();
		buf[i] = character;
		if (!(response[i] == character))
			return 0;
	}
	return 1;
}


void http_getrequest(char * uri, int requestState) {
	//parse the url from the uri
	char *url = strtok(uri, "/");
	//get the resource information from the uri
	char *resource = strtok(NULL," ");
	//int getLength = 5 + strlen(resource) + 6; // 5 for "GET /" + strlen(resource) for the resource + 9 for " HTTP/1.1" + 6 for "Host: " + 6 for all the newline and carriage return characters
	int getLength = 5 + strlen(resource) + 9 + 6 + strlen(url) + 6;
	//int getLength = 26 + 6 + 23 + 4;

	//connect the to the ip of the server
	if (requestState == 0) {
		wifi_sendstring("AT+CIPSTART=\"TCP\",\"");
		wifi_sendstring(url);
		wifi_sendstring("\",80\r\n");
	}

	//sends the number of bytes in the the get request (AT+CIPSEND)
	if (requestState == 1) {
		wifi_sendstring("AT+CIPSEND=");
		char buff[2];
		itoa(getLength, buff,10);
		wifi_sendstring(buff);
		wifi_sendstring("\r\n");
	}

	//send the get request
	if (requestState == 2) {
/*
		//form the HTTP get request and send it
		wifi_sendstring("GET /");
		wifi_sendstring(resource);
		wifi_sendstring("\r\n\r\n\r\n");
		//write to the screen that it is done
		textWrite(resource, strlen(resource));
*/
		//form the HTTP get request and send it
		wifi_sendstring("GET /");
		wifi_sendstring(resource);
		wifi_sendstring(" HTTP/1.1\r\nHost: ");
		wifi_sendstring(url);
		wifi_sendstring("\r\n\r\n");
		//write to the screen that it is done
		//textWrite(resource, strlen(resource));




		//int getLength = 26 + 6 + 23 + 4;
		//wifi_sendstring("GET /api/json/est/now HTTP/1.1\r\nHost: worldclockapi.com\r\n\r\n");
	}

}


/*
 *
 *sample code that i wrote in a different file. Just so I could use it as a reference
		wifi_sendstring("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
		tim6_changeTimer(3000);
	}
	if (state == 4) {
		wifi_sendstring("AT+CIPSEND=52\r\n");
		tim6_changeTimer(1000);
	}
	if (state == 5)	{
		wifi_sendstring("GET /update?api_key=2155L8AXXZLPF57M&field1=42\r\n\r\n\r\n");	}
	if (state <= 5)
		state++;
*/



void wifi_parseresponse(char * http) {
	/*//worldclockapi.com
	char *response = strstr(http, "\r\n\r\n")+4;
	char *datetime = strstr(response, "\"currentDateTime\":")+18;
	char *time = strstr(datetime, "T")+1;
	time[2] = '\0';
	time[5] = '\0';
	// might not work because passing indexing the string returns a char
	// while atoi is expecting a pointer
	//hour = atoi(&time[0])*10 + atoi();
	//minute = atoi("3") * 10 + atoi("3");
	hour = atoi(&time[0]);
	minute = atoi(&time[3]);
	*/

	char *response = strstr(http, "\r\n\r\n")+4;
	char *datetime = strstr(response, "datetime");
	char *time = strstr(datetime, " ")+1;

	time[2] = '\0';
	time[5] = '\0';
	time[8] = '\0';
	hour = atoi(&time[0]);
	minute = atoi(&time[3]);
	second = atoi(&time[6]);

}


volatile char wifi_readbuff[10] = "";
volatile char wifi_response[800];
volatile char responseStateIPD = 0;
volatile char responseStateOK = 0;
volatile char responseStateDisconnect = 0;
volatile int responseBytesToGo = 0;
volatile int responseBytesTotal = 0;

void wifi_clearreadbuff(void) {
	for (int i = 10; i > 0; i--)
		wifi_readbuff[i] = '\0';
}

void USART1_IRQHandler(void) {
	if (USART1->ISR & USART_ISR_RXNE) {
		uint8_t rxByte = wifi_getchar();

		//state machine to recognize "+IPD," and then the number that comes after it
		//which is the size of the incoming HTTP response in bytes (including /n and /r)
		switch(responseStateIPD) {
			case 0 : if (rxByte == '+')responseStateIPD++;
				else {responseStateIPD = 0;}
				break;
			case 1 : if (rxByte == 'I')responseStateIPD++;
				else {responseStateIPD = 0;}
				break;
			case 2 : if (rxByte == 'P')responseStateIPD++;
				else {responseStateIPD = 0;}
				break;
			case 3 : if (rxByte == 'D')responseStateIPD++;
				else {responseStateIPD = 0;}
				break;
			case 4 : if (rxByte == ',')responseStateIPD++;
				else {responseStateIPD = 0;}
				break;
			case 5 :;
				//check the number of bytes that are needed to be read
				int i;
				for (i = 0; (rxByte != ':' && i < 10);  i++) {
					wifi_readbuff[i] = rxByte;
					rxByte = wifi_getchar();
				}
				wifi_readbuff[i] = '\0';
				//get responseBytesToGo and responseBytesTotal from the string
				responseBytesTotal = atoi(wifi_readbuff);
				responseBytesToGo = responseBytesTotal;

				//increment the state machine
				responseStateIPD++;
				break;
			case 6 :
				//reads the response one byte at a time
				if (responseBytesToGo != 0) {
					wifi_response[responseBytesTotal - responseBytesToGo] = rxByte;
					responseBytesToGo--;
				}
				if (responseBytesToGo == 0) {
					//textWrite(wifi_response, 537);

					wifi_response[responseBytesTotal] = '\0';
					//call some parsing function
					wifi_parseresponse(wifi_response);

					//hour = 5;
					//minute = 10;
					//fix state variables
					responseStateIPD = 0;
					responseBytesTotal = 0;

					//clear wifi_readbuff
					//wifi_clearreadbuff();
					//USART1->RQR |= USART_RQR_RXFRQ;
				}
				break;
			default: responseStateIPD = 0;
		}

		switch(responseStateOK) {
					case 0 : if (rxByte == '\r' || rxByte == '\n')responseStateOK++;
						else {responseStateOK = 0;}
						break;
					case 1 : if (rxByte == '\r' || rxByte == '\n')responseStateOK++;
						else {responseStateOK = 0;}
						break;
					case 2 : if (rxByte == 'O')responseStateOK++;
						else if (rxByte == '\r' || rxByte == '\n')responseStateOK=2;
						else {responseStateOK = 0;}
						break;
					case 3 : if (rxByte == 'K')responseStateOK++;
						else {responseStateOK = 0;}
						break;
					case 4 : if (rxByte == '\r' || rxByte == '\n')responseStateOK++;
						else {responseStateOK = 0;}
						break;
					case 5 : if (rxByte == '\r' || rxByte == '\n')responseStateOK++;
						else {responseStateOK = 0;}
						//recognized that an OK has been received

						//if wifi is in initialization state, then increment the wifi initial state
						//and trigger the timer 6 interrupt
						if (tim6semaphore == 0) {
							wifiInitialState++;
							tim6_triggerInterrupt();
						}
						//if wifi is in initialization state, then increment the wifi http state
						//and trigger the timer 6 interrupt
						if (tim6semaphore == 1) {
							wifiHTTPState++;
							tim6_triggerInterrupt();
						}
						//reset the response state variable
						responseStateOK = 0;
						break;
			default: responseStateOK = 0;
		}

		switch(responseStateDisconnect) {
					case 0 : if (rxByte == 'W')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 1 : if (rxByte == 'I')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 2 : if (rxByte == 'F')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 3 : if (rxByte == 'I')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 4 : if (rxByte == ' ')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 5 : if (rxByte == 'D')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 6 : if (rxByte == 'I')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 7 : if (rxByte == 'S')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 8 : if (rxByte == 'C')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 9 : if (rxByte == 'O')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 10 : if (rxByte == 'N')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 11 : if (rxByte == 'N')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 12 : if (rxByte == 'E')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 13 : if (rxByte == 'C')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						break;
					case 14 : if (rxByte == 'T')responseStateDisconnect++;
						else {responseStateDisconnect = 0;}
						//WIFI DISCONNECT is read. Must reconnect to the wifi if the timer 6 semaphore is not 0
						if (tim6semaphore != 0) {
							//reset the http state
							wifiHTTPState = 0;
							//reset the timer semaphore
							tim6semaphore = 0;
							//reset the wifi intial state to redo the connection
							wifiInitialState = 2;

						}
						responseStateDisconnect = 0;

						break;


			default: responseStateDisconnect = 0;
		}
	}

}


