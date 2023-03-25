/*
 * esp.h
 *
 *  Created on: Feb 3, 2023
 *      Author: mrahf
 */

#ifndef ESP_H_
#define ESP_H_

void setup_uart1();
uint8_t wifi_sendchar(int txChar);
uint8_t wifi_getchar(void);
char * wifi_sendstring(char * cmd);
char wifi_checkstring(char * response);
void http_getrequest(char * uri, int requestState);
#endif /* ESP_H_ */
