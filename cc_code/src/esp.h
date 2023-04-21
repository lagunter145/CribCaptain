/*
 * esp.h
 *
 *  Created on: Feb 3, 2023
 *      Author: mrahf
 */

#ifndef ESP_H_
#define ESP_H_

char readBuffer[10];

extern volatile int minute;
extern volatile int hour;
extern volatile int second;
extern volatile int dayOfWeek;

extern char refreshState;
extern volatile int wifiHTTPState;
extern volatile int wifiInitialState;
extern volatile int wifiTimeHTTPState;
extern volatile int tim6semaphore;


void setup_uart1();
uint8_t wifi_sendchar(uint8_t txChar);
uint8_t wifi_getchar(void);
char * wifi_sendstring(char * cmd);
char wifi_checkstring(char * response);
void http_setupcheckin(char * uid, uint8_t checkedIn, uint8_t numGuest);
void http_getrequest(char * uri, int requestState);
void wifi_clearreadbuff(void);
void wifi_parseresponse(volatile char * http);
void tim6_triggerInterrupt(void);
void http_refresh(uint8_t state);


#endif /* ESP_H_ */


