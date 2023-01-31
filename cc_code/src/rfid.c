/*
 * rfid.c
 *
 *  Created on: Jan 27, 2023
 *      Author: phili
 */

#include "stm32f0xx.h"
#include "rfid.h"

// USART5 was used from 362 Lab 10. I just copied over the initiation and
// basic read/write functions.
// According to the STM32F0 Reference Manual, USART5/6/7/8 are missing a
// lot of features (don't know if we actually need). USART1/2 are fully featured.
// USART1_TX: PA9 AF1, PB6 AF0
// USART1_RX: PA10 AF1, PB7 AF0
//***** CHANGE TO USART1 *****//
void init_usart5()
{
    // enable RCC clock to GPIOC and GPIOD
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
    // config PC12 to be routed to USART5_TX (AF2)
    GPIOC->MODER &=~0x03000000;
    GPIOC->MODER |= 0x02000000;
    GPIOC->AFR[1] |=0x00020000;
    // config PD2 to be routed to USART5_RX (AF2)
    GPIOD->MODER &=~0x00000030;
    GPIOD->MODER |= 0x00000020;
    GPIOD->AFR[0] |=0x00000200;
    // enable RCC clock to USART5
    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;

    // configure USART5
    USART5->CR1 &=~USART_CR1_UE;    // disable USART5
    USART5->CR1 &=~USART_CR1_M;     // set word size of 8 bits
    USART5->CR1 &=~(1<<28);
    USART5->CR2 &=~USART_CR2_STOP;  // set 1 stop bit
    USART5->CR1 &=~USART_CR1_PCE;   // set no parity
    USART5->CR1 &=~USART_CR1_OVER8; // use 16x oversampling
    USART5->BRR = 0;                // use baud rate 115200
    USART5->BRR |= 0x1a1;
    USART5->CR1 |= USART_CR1_TE | USART_CR1_RE; // enable transmitter and receiver by setting TE and RE
    USART5->CR1 |= USART_CR1_UE;    // enable USART
    // check TEACK and REACK bits of ISR to be set
    while(!((USART5->ISR & USART_ISR_TEACK) && (USART5->ISR & USART_ISR_REACK)));
}
// is multibuffer communication needed? If so, must also enable DMA

// overrun error (ORE bit) can occur if the RXNE flag is set when the next data is received.
    // interrupt only sent if RXNEIE or EIE bit is set
// framing error (FE bit) can occur if stop bit not recognized
    // no interrupt is generated for single byte comm.
    // if EIE bit set in CR3 and multibuffer comm, then interrupt is sent


int write_byte(int c)
{
    // wait for TXE bit to be set
    while (!(USART5->ISR & USART_ISR_TXE)) {}

    USART5->TDR = c;

    return c;
}
// pg 687: "8. After writing the last data into the USARTx_TDR register, wait
// until TC=1. This indicates that the transmission of the last frame is complete.
// This is required for instance when the USART is disabled or enters the Halt
// mode to avoid corrupting last transmission.

int read_byte(void)
{
    // wait for RXNE bit to be set
    while (!(USART5->ISR & USART_ISR_RXNE)) {}
    int c = USART5->RDR;

    return c;
}

/*
 * THE FOLLOWING FUNCTIONS ARE LARGELY TAKEN FROM ELECHOUSE'S C++ LIBRARY ON
 * GITHUB, TRANSLATED TO C SYNTAX.
 * https://github.com/elechouse/PN532/blob/PN532_HSU/PN532/PN532.cpp
*/


/**
    @brief receive data .
    @param buf --> return value buffer.
           len --> length expect to receive.
           timeout --> time of reveiving
    @retval number of received bytes, 0 means no data received.
*/
// pass timeout=0 to avoid timeout checks
int8_t receive(uint8_t *buf, int len, uint16_t timeout)
{
  int read_bytes = 0;
  int ret;
  //unsigned long start_millis;

  while (read_bytes < len) {
    //start_millis = millis();
    do {
      ret = read_byte();
      if (ret >= 0) {
        break;
     }
    } while((timeout == 0) /*|| ((millis()- start_millis ) < timeout)*/);

    if (ret < 0) {
        if(read_bytes){
            return read_bytes;
        }else{
            return PN532_TIMEOUT;
        }
    }
    buf[read_bytes] = (uint8_t)ret;
    //DMSG_HEX(ret);
    read_bytes++;
  }
  return read_bytes;
}

int8_t readAckFrame()
{
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
    uint8_t ackBuf[sizeof(PN532_ACK)];

    //DMSG("\nAck: ");

    if( receive(ackBuf, sizeof(PN532_ACK), 0 /*PN532_ACK_WAIT_TIME*/) <= 0 ){
        //DMSG("Timeout\n");
        return PN532_TIMEOUT;
    }

    if( memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK)) ){
        //DMSG("Invalid\n");
        return PN532_INVALID_ACK;
    }
    return 0;
}

int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{

    /** dump serial buffer
    if(_serial->available()){
        DMSG("Dump serial buffer: ");
    }
    while(_serial->available()){
        uint8_t ret = _serial->read();
        DMSG_HEX(ret);
    }*/

    command = header[0];

    write_byte(PN532_PREAMBLE);
    write_byte(PN532_STARTCODE1);
    write_byte(PN532_STARTCODE2);

    uint8_t length = hlen + blen + 1;   // length of data field: TFI + DATA
    write_byte(length);
    write_byte(~length + 1);         // checksum of length

    write_byte(PN532_HOSTTOPN532);
    uint8_t sum = PN532_HOSTTOPN532;    // sum of TFI + DATA

    //DMSG("\nWrite: ");

    for(int i = 0; i < hlen; i++) {
        write_byte(header[i]);
    }
    for (uint8_t i = 0; i < hlen; i++) {
        sum += header[i];

        //DMSG_HEX(header[i]);
    }

    for(int j = 0; j < blen; j++) {
        write_byte(body, blen);
    }
    for (uint8_t i = 0; i < blen; i++) {
        sum += body[i];

        DMSG_HEX(body[i]);
    }

    uint8_t checksum = ~sum + 1;            // checksum of TFI + DATA
    write_byte(checksum);
    write_byte(PN532_POSTAMBLE);

    return readAckFrame();
}


int16_t readResponse(uint8_t * buf, uint8_t len, uint16_t timeout)
{
    uint8_t tmp[3];

    //DMSG("\nRead:  ");

    /** Frame Preamble and Start Code */
    if(receive(tmp, 3, timeout)<=0){
        return PN532_TIMEOUT;
    }
    if(0 != tmp[0] || 0!= tmp[1] || 0xFF != tmp[2]){
        //DMSG("Preamble error");
        return PN532_INVALID_FRAME;
    }

    /** receive length and check */
    uint8_t length[2];
    if(receive(length, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( 0 != (uint8_t)(length[0] + length[1]) ){
        //DMSG("Length error");
        return PN532_INVALID_FRAME;
    }
    length[0] -= 2;
    if( length[0] > len){
        return PN532_NO_SPACE;
    }

    /** receive command byte */
    uint8_t cmd = command + 1;               // response command
    if(receive(tmp, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( PN532_PN532TOHOST != tmp[0] || cmd != tmp[1]){
        //DMSG("Command error");
        return PN532_INVALID_FRAME;
    }

    if(receive(buf, length[0], timeout) != length[0]){
        return PN532_TIMEOUT;
    }
    uint8_t sum = PN532_PN532TOHOST + cmd;
    for(uint8_t i=0; i<length[0]; i++){
        sum += buf[i];
    }

    /** checksum and postamble */
    if(receive(tmp, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( 0 != (uint8_t)(sum + tmp[0]) || 0 != tmp[1] ){
        //DMSG("Checksum error");
        return PN532_INVALID_FRAME;
    }

    return length[0];
}

// input arg 'reg' is the 16 bit register address
// returns register value
int readRegister(uint16_t reg)
{
    int response;
    // in order to read, have to first write a READREGISTER command
    pn532_packetbuffer[0] = PN532_COMMAND_READREGISTER;
    pn532_packetbuffer[1] = (reg >> 8) & 0xFF;
    pn532_packetbuffer[2] = reg & 0xFF;

    if (writeCommand(pn532_packetbuffer, 3)) {
        return 0;
    }

    // read data packet
    int16_t status = readResponse(pn532_packetbuffer, sizeof(pn532_packetbuffer), 0);
    if (0 > status) {
        return 0;
    }

    response = pn532_packetbuffer[0];

    return response;
}

// input arg 'reg' is the 16 bit register address
// input arg 'val' is the 8 bit value to write
// returns 0 for failure, 1 for success
int writeRegister(uint16_t reg, uint8_t val)
{
    int response;

    pn532_packetbuffer[0] = PN532_COMMAND_WRITEREGISTER;
    pn532_packetbuffer[1] = (reg >> 8) & 0xFF;
    pn532_packetbuffer[2] = reg & 0xFF;
    pn532_packetbuffer[3] = val;


    if (writeCommand(pn532_packetbuffer, 4)) {
        return 0;
    }

    // read data packet
    int16_t status = readResponse(pn532_packetbuffer, sizeof(pn532_packetbuffer), 0);
    if (0 > status) {
        return 0;
    }

    return 1;
}

/**************************************************************************/
/*!
    Waits for an ISO14443A target to enter the field
    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
int readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout)
{
    pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
    pn532_packetbuffer[2] = cardbaudrate;

    if (writeCommand(pn532_packetbuffer, 3)) {
        return 0x0;  // command failed
    }

    // read data packet
    if (readResponse(pn532_packetbuffer, sizeof(pn532_packetbuffer), 0) < 0) {
        return 0x0;
    }

    // check some basic stuff
    /* ISO14443A card response should be in the following format:
      byte            Description
      -------------   ------------------------------------------
      b0              Tags Found
      b1              Tag Number (only one used in this example)
      b2..3           SENS_RES
      b4              SEL_RES
      b5              NFCID Length
      b6..NFCIDLen    NFCID
    */

    if (pn532_packetbuffer[0] != 1)
        return 0;

    uint16_t sens_res = pn532_packetbuffer[2];
    sens_res <<= 8;
    sens_res |= pn532_packetbuffer[3];

    /*DMSG("ATQA: 0x");  DMSG_HEX(sens_res);
    DMSG("SAK: 0x");  DMSG_HEX(pn532_packetbuffer[4]);
    DMSG("\n");*/

    /* Card appears to be Mifare Classic */
    *uidLength = pn532_packetbuffer[5];

    for (uint8_t i = 0; i < pn532_packetbuffer[5]; i++) {
        uid[i] = pn532_packetbuffer[6 + i];
    }

    return 1;
}