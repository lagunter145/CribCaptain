/*
 * rfid.h
 *
 *  Created on: Jan 27, 2023
 *      Author: phili
 */

#ifndef RFID_H_
#define RFID_H_
#include "stdlib.h"
// buffer to hold values to send/receive to/from the PN532
uint8_t pn532_packetbuffer[64];

// byte that holds command for writing to PN532
uint8_t command;

// flag for indicating a card has been scanned;
uint8_t card_scanned;

// function declarations
void init_usart5();
void enable_DMA1();
void wakeup();
void write_byte(uint8_t c);
uint8_t read_byte(void);
int8_t receive(uint8_t * buf, int len, uint16_t timeout);
int8_t readAckFrame();
int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
int16_t readResponse(uint8_t * buf, uint8_t len, uint16_t timeout);

uint32_t readRegister(uint16_t reg);
uint32_t writeRegister(uint16_t reg, uint8_t val);

uint32_t getFirmwareVersion(void);
int SAMConfig(void);
uint8_t setPassiveActivationRetries(uint8_t maxRetries);
int readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout);

// Command Codes
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)

#define PN532_MIFARE_ISO14443A              (0x00)

// from PN532Interface
#define PN532_PREAMBLE                (0x00)
#define PN532_STARTCODE1              (0x00)
#define PN532_STARTCODE2              (0xFF)
#define PN532_POSTAMBLE               (0x00)

#define PN532_HOSTTOPN532             (0xD4)
#define PN532_PN532TOHOST             (0xD5)

#define PN532_ACK_WAIT_TIME           (10)  // ms, timeout of waiting for ACK

#define PN532_INVALID_ACK             (-1)
#define PN532_TIMEOUT                 (-2)
#define PN532_INVALID_FRAME           (-3)
#define PN532_NO_SPACE                (-4)

#define REVERSE_BITS_ORDER(b)         b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; \
                                      b = (b & 0xCC) >> 2 | (b & 0x33) << 2; \
                                      b = (b & 0xAA) >> 1 | (b & 0x55) << 1

#endif /* RFID_H_ */
