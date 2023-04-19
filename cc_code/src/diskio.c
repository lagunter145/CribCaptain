
#include "stm32f0xx.h"
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

// things to turn turn on/off the NSS line.
void enable_sdcard() {
    GPIOA->BSRR = GPIO_BSRR_BR_1;
}
void disable_sdcard() {
    GPIOA->BSRR = GPIO_BSRR_BS_1;
}

void spi_clear_rxfifo(SPI_TypeDef *s)
{
    while(s->SR & SPI_SR_RXNE) {
        // clear the read buffer
        int __attribute__((unused))dummy = *(uint8_t*)&(s->DR);
    }
}

uint8_t sdcard_write(uint8_t b)
{
    while((SPI1->SR & SPI_SR_TXE) == 0)
        ;
    *((uint8_t*)&(SPI1->DR)) = b;
    int value = 0xff;
    while ((SPI1->SR & SPI_SR_RXNE) != SPI_SR_RXNE)
        ;
        value = *(uint8_t *)&(SPI1->DR);
    while((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY)
        ;
    return value;
}

void sdcard_init_clock()
{
    for(int i=0; i<10; i++)
        sdcard_write(0xff);
}

int sdcard_cmd(uint8_t cmd, uint32_t arg, int crc)
{
    sdcard_write(64 + cmd);
    sdcard_write((arg>>24) & 0xff);
    sdcard_write((arg>>16) & 0xff);
    sdcard_write((arg>>8) & 0xff);
    sdcard_write((arg>>0) & 0xff);
    sdcard_write(crc);
    int value = 0xff;
    int count=0;
    // The card should respond to any command within 8 bytes.
    // We'll wait for 100 to be safe.
    for(; count<100; count++) {
        value = sdcard_write(0xff);
        if (value != 0xff) break;
    }
    return value;
}

int sdcard_r3(void)
{
    int value = 0;
    value = (value << 8) | sdcard_write(0xff);
    value = (value << 8) | sdcard_write(0xff);
    value = (value << 8) | sdcard_write(0xff);
    value = (value << 8) | sdcard_write(0xff);
    return value;
}

int sdcard_readblock(BYTE buffer[], int len)
{
    int value = 0xff;
    int count=0;
    for(; count<100000; count++) {
        value = sdcard_write(0xff);
        if (value != 0xff) break;
    }
    if (value != 0xfe)
        return value;
    for(int i=0; i<len; i++)
        buffer[i] = sdcard_write(0xff);
    uint8_t crc1 = sdcard_write(0xff);
    uint8_t crc2 = sdcard_write(0xff);
    uint8_t check = sdcard_write(0xff); // Check that this is 0xff
    return 0xfe;
}

int sdcard_writeblock(BYTE buffer[], int len)
{
    int value = 0xff;
    value = sdcard_write(0xff); // pause for one byte [expect 0xff]
    value = sdcard_write(0xfe); // start data packet [expect 0xff]
    for(int i=0; i<len; i++)
        value = sdcard_write(buffer[i]);
    value = sdcard_write(0x01); // write the crc [expect 0xff]
    value = sdcard_write(0x01); // write the crc [expect 0xff]
    do {
        value = sdcard_write(0xff); // Get the response [exepct xxx00101 == 0x1f & 0x05]
    } while(value == 0xff);
    int status = value & 0x1f;
    do {
        value = sdcard_write(0xff);
    } while(value != 0xff);
    return status;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

void setup_spi1();
void spi_high_speed();

static DSTATUS sdcard_status = STA_NOINIT;

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
    int value;
    // how many times have we tried?
    int count = 0;
    restart:
    count++;
    if (count > 10)
        return STA_NOINIT;
    setup_spi1();
    disable_sdcard();
    sdcard_init_clock();
    spi_high_speed();
    spi_clear_rxfifo(SPI1);
    enable_sdcard();
    value = sdcard_cmd(0, 0x00000000, 0x95); // Go to idle state
//    for (;;) {
//        value = sdcard_cmd(0, 0x00000000, 0x95); // Go to idle state
//        int z = 0;
//        while (z < 1000)
//            z++;
//    }
    if (value != 1)
        goto restart;
    disable_sdcard();

    enable_sdcard();
    value = sdcard_cmd(8, 0x000001aa, 0x87); // Check voltage range
    value = sdcard_r3();
    disable_sdcard();

    do {
        enable_sdcard();
        value = sdcard_cmd(55, 0x40000000, 0x01); // Start initialization
        value = sdcard_cmd(41, 0x40000000, 0x01); // Start initialization
        disable_sdcard();
    } while(value != 0);

    enable_sdcard();
    value = sdcard_cmd(58, 0x00000000, 0x01); // get OCR [expect 0x00]
    value = sdcard_r3(); // read OCR value [expect 0xc0ff8000]
    disable_sdcard();

    enable_sdcard();
    value = sdcard_cmd(16, 0x00000200, 0x01); // set block size [expect 0x00]
    disable_sdcard();

    sdcard_status = RES_OK;
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    if (sdcard_status != 0) {
        sdcard_status = disk_initialize(pdrv);
    }
    // Read the OCR to check if the card is still accessible.
    enable_sdcard();
    if (sdcard_cmd(58, 0x00000000, 0x01) == 0) {
        sdcard_r3();
        return RES_OK;
    }
    disable_sdcard();

    sdcard_status = STA_NOINIT;
    return disk_initialize(pdrv);
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buffer,	/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
    int value;
    int status = RES_OK;
    if (disk_status(pdrv) == STA_NOINIT)
        return RES_NOTRDY;
    enable_sdcard();
    for(int c=0; c<count; c++) {
        BYTE *p = &buffer[512 * c];
        value = sdcard_cmd(17, sector+c, 0x01);
        if (value != 0) {
            status = RES_ERROR;
            break;
        }
        value = sdcard_readblock(p,512);
        if (value != 0xfe) {
            status = RES_ERROR;
            break;
        }
    }
    disable_sdcard();
    return status;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buffer,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
    BYTE *p;
    int value;
    int status = RES_OK;
    if (disk_status(pdrv) == STA_NOINIT)
        return RES_NOTRDY;
    enable_sdcard();
    for(int c=0; c<count; c++) {
        const BYTE *p = &buffer[512 * c];
        value = sdcard_cmd(24, sector+c, 0x01);
        if (value != 0) {
            status = RES_ERROR;
            break;
        }
        value = sdcard_writeblock(p,512);
        if (value != 0x05) {
            status = RES_ERROR;
            break;
        }
    }
    disable_sdcard();
    return status;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_ERROR;
	BYTE n, csd[16];
	int result;

	if (disk_status(pdrv) & STA_NOINIT)
	    return RES_NOTRDY;

	switch (cmd) {
	case CTRL_SYNC: // make sure there is no pending write process
	    return RES_OK; // can't really do much with this now

	case GET_SECTOR_COUNT: // Get the number of sectors on the disk
	    if (sdcard_cmd(9, 0x00000000, 0x1) != 0)
	        break;
	    else {
	        sdcard_readblock(csd, 16);
	        int cs = csd[9] + (((int)csd[8])<<8) + (((int)csd[7])<<16) + 1;
	        *(int*)buff = cs << 9;
	        return RES_OK;
	    }

	case GET_BLOCK_SIZE: // Get the erase block size (in sectors)
	    *(int *)buff = 256; // let's go with 128 kb
	    return RES_OK;

	default:
	    return RES_PARERR;
	}
	return res;
}

