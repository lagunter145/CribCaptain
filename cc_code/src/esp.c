/*
 * esp.c
 *
 *  Created on: Feb 3, 2023
 *      Author: mrahf
 */


#include "stm32f0xx.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "timer.h"
#include "lcd_7in.h"
#include "esp.h"
#include "roommates.h"
#include "misc.h"

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

	//Baud rate of 115200
	USART1->BRR = 0;
	USART1->BRR |= 0x1A1;
	//enable the USART1
	USART1->CR1 |= USART_CR1_UE;
	//Enable the transmitter and receiver
	USART1->CR1 |= USART_CR1_RE | USART_CR1_TE;

	//Wait for the TE and RE bits to be acknowledged
	while(!((USART1->ISR & USART_ISR_TEACK) && (USART1->ISR & USART_ISR_REACK)));
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

volatile char url[200] = "";
const char serverURL[] = "192.168.193.87";
//192.168.175.87/checkin.php/?uid=2&checkedIn=9&numGuest=12
void http_setupcheckin(char * uid, uint8_t checkedIn, uint8_t numGuest) {
	char buff[3];
	//zero the contnets of the url string
	memset(url, 0, strlen(url));
	strcat(url, serverURL);
	strcat(url, "/checkin.php/?uid=");
	strcat(url, uid);
	strcat(url, "&checkedIn=");
	itoa(checkedIn, buff, 10);
	strcat(url, buff);
	strcat(url, "&numGuest=");
	itoa(numGuest, buff, 10);
	strcat(url,buff);
	//
	wifiTimeHTTPState = 0;
	wifiHTTPState = 0;
	tim6semaphore =2;
	tim6_triggerInterrupt();

}

char refreshState = '0';
void http_refresh(uint8_t state) {
	//update time
	if (state == 0) {
		refreshState = '0';
		wifiTimeHTTPState = 0;
		tim6semaphore = 1;
	}
	//update roomate data
	if (state == 1) {
		refreshState = '1';
		wifiTimeHTTPState = 0;
		tim6semaphore = 1;
	}
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
		//form the HTTP get request and send it
		wifi_sendstring("GET /");
		wifi_sendstring(resource);
		wifi_sendstring(" HTTP/1.1\r\nHost: ");
		wifi_sendstring(url);
		wifi_sendstring("\r\n\r\n");
	}
}


volatile char wifi_readbuff[10] = "";
volatile char wifi_response[1500];
volatile char responseStateIPD = 0;
volatile char responseStateOK = 0;
volatile char responseStateDisconnect = 0;
volatile char responseStateConnect = 0;
volatile char wifiConnected = 0;
volatile int responseBytesToGo = 0;
volatile int responseBytesTotal = 0;
volatile char timeAcquired = 0;

void wifi_clearreadbuff(void) {
	for (int i = 10; i > 0; i--)
		wifi_readbuff[i] = '\0';
}

void wifi_parseresponse(volatile char * http) {
	char *response = strstr(http, "\r\n\r\n")+4;
	char *datetime = strstr(response, "datetime");
	char *time = strstr(datetime, " ")+1;

	time[2] = '\0';
	time[5] = '\0';
	time[8] = '\0';
	hour = atoi(&time[0]);
	minute = atoi(&time[3]);
	second = atoi(&time[6]);

	timeAcquired = 1;
}

// function to parse out events from an http response
void wifi_parseresponse_events(volatile char * http, char refreshState) {

	if (refreshState == '0') {
		//parse the time first
		char timeBuff[2];
		char *time = strstr(http, "\"time\"=\"") + 8;
		strncpy(timeBuff, time, 2);
		hour = atoi(timeBuff);
		time = time + 3;
		strncpy(timeBuff, time, 2);
		minute = atoi(timeBuff);
		time = time + 3;
		strncpy(timeBuff, time, 2);
		second = atoi(timeBuff);

		timeAcquired = 1;
	}

	if (refreshState == '1') {
		//parse the rest of the update message
		char *rmates[4];
		rmates[0] = strstr(http, "name") + 4;
		for (int i = 0; i < 3; i++)
		  rmates[i+1] = strstr(rmates[i], "name") + 4;

		for (int i = 0; i < 4; i++) {
			char buff[20] = "";
			//get the number of Guests
			char * numGuestStr = strstr(rmates[i], "numGuest") + 11;
			char * numGuestStrEnd = strstr(numGuestStr, "\"");
			strncpy(buff, numGuestStr ,numGuestStrEnd-numGuestStr);
			int numGuest = atoi(buff);
			roommates[i].num_guests = numGuest;

			//get the checkedIn
			memset(buff,0,strlen(buff));
			char * checkedInStr = strstr(rmates[i], "checkedIn") + 12;
			char * checkedInStrEnd = strstr(checkedInStr, "\"");
			strncpy(buff, checkedInStr , checkedInStrEnd-checkedInStr);
			int checkedIn = atoi(buff);
			roommates[i].home = checkedIn;
			if (checkedIn) {
				set_pin(GPIOA, (i + 5), 1);
			}

			//get event1
			memset(buff,0,strlen(buff));
			char * event1Str = strstr(rmates[i], "event1") + 9;
			char * event1StrEnd = strstr(event1Str, "\"");
			strncpy(buff, event1Str , event1StrEnd-event1Str);
			strcpy(roommates[i].events[0].name, buff);
			printf("%s\n", buff);


			//get event2
			memset(buff,0,strlen(buff));
			char * event2Str = strstr(rmates[i], "event2") + 9;
			char * event2StrEnd = strstr(event2Str, "\"");
			strncpy(buff, event2Str , event2StrEnd-event2Str);
			strcpy(roommates[i].events[1].name, buff);
			printf("%s\n", buff);

			//get edow1
			memset(buff,0,strlen(buff));
			char * edow1Str = strstr(rmates[i], "edow1") + 8;
			char * edow1StrEnd = strstr(edow1Str, "\"");
			char edow1 = edow1Str[0];
			roommates[i].events[0].day = edow1;
			//strcpy(roommates[0].events[0].day, buff);

			//get edow2
			memset(buff,0,strlen(buff));
			char * edow2Str = strstr(rmates[i], "edow2") + 8;
			char * edow2StrEnd = strstr(edow2Str, "\"");
			char edow2 = edow2Str[0];
			roommates[i].events[1].day = edow2;

			timeAcquired = 1;

		}
    }


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
					//if the device is receiving the time, call some parsing function
					if (tim6semaphore == 1) {
						wifi_parseresponse_events(wifi_response, refreshState);
						wifi_sendstring("AT+CIPCLOSE\r\n");
					}

					//fix state variables
					responseStateIPD = 0;
					responseBytesTotal = 0;
					//for (int i = 0; i<1500; i++)
					//	wifi_response[i] = '\0';
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
				//if wifi is establishing the time, then increment the wifi time state
				//and trigger the timer 6 interrupt
				if (tim6semaphore == 1) {
					wifiTimeHTTPState++;
					tim6_triggerInterrupt();
				}
				//if wifi is sending a normal http request, then increment the state
				//and trigger the timer 6 interrupt
				if (tim6semaphore == 2) {

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
				wifiConnected = 0; //wifi is not connected
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


		switch(responseStateConnect) {
			case 0 : if (rxByte == 'W')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 1 : if (rxByte == 'I')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 2 : if (rxByte == 'F')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 3 : if (rxByte == 'I')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 4 : if (rxByte == ' ')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 5 : if (rxByte == 'C')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 6 : if (rxByte == 'O')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 7 : if (rxByte == 'N')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 8 : if (rxByte == 'N')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 9 : if (rxByte == 'E')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 10 : if (rxByte == 'C')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 11 : if (rxByte == 'T')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 12 : if (rxByte == 'E')responseStateConnect++;
				else {responseStateConnect = 0;}
				break;
			case 13 : if (rxByte == 'D')responseStateConnect++;
				else {responseStateConnect = 0;}
				//WIFI CONNECT is read. Update Wifi Connected state variable
				wifiConnected = 1;
				responseStateConnect = 0;
				break;
			default: responseStateConnect = 0;
		}
	}

}


