/*
 * RA8775_commands.h
 *
 *  Created on: Feb 8, 2023
 *      Author: Lauren
 */

#ifndef RA8775_COMMANDS_H_
#define RA8775_COMMANDS_H_

// Colors (RGB565)
#define RA8875_BLACK 0x0000   ///< Black Color
#define RA8875_BLUE 0x001F    ///< Blue Color
#define RA8875_RED 0xF800     ///< Red Color
#define RA8875_GREEN 0x07E0   ///< Green Color
#define RA8875_CYAN 0x07FF    ///< Cyan Color
#define RA8875_MAGENTA 0xF81F ///< Magenta Color
#define RA8875_YELLOW 0xFFE0  ///< Yellow Color
#define RA8875_WHITE 0xFFFF   ///< White Color

// Command/Data pins for SPI
#define RA8875_DATAWRITE 0x00 ///< See datasheet
#define RA8875_DATAREAD 0x40  ///< See datasheet
#define RA8875_CMDWRITE 0x80  ///< See datasheet
#define RA8875_CMDREAD 0xC0   ///< See datasheet

// Registers & bits
#define RA8875_PWRR 0x01           ///< See datasheet
#define RA8875_PWRR_DISPON 0x80    ///< See datasheet
#define RA8875_PWRR_DISPOFF 0x00   ///< See datasheet
#define RA8875_PWRR_SLEEP 0x02     ///< See datasheet
#define RA8875_PWRR_NORMAL 0x00    ///< See datasheet
#define RA8875_PWRR_SOFTRESET 0x01 ///< See datasheet

#define RA8875_MRWC 0x02 ///< See datasheet

#define RA8875_GPIOX 0xC7 ///< See datasheet

#define RA8875_PLLC1 0x88         ///< See datasheet
#define RA8875_PLLC1_PLLDIV2 0x80 ///< See datasheet
#define RA8875_PLLC1_PLLDIV1 0x00 ///< See datasheet

#define RA8875_PLLC2 0x89        ///< See datasheet
#define RA8875_PLLC2_DIV1 0x00   ///< See datasheet
#define RA8875_PLLC2_DIV2 0x01   ///< See datasheet
#define RA8875_PLLC2_DIV4 0x02   ///< See datasheet
#define RA8875_PLLC2_DIV8 0x03   ///< See datasheet
#define RA8875_PLLC2_DIV16 0x04  ///< See datasheet
#define RA8875_PLLC2_DIV32 0x05  ///< See datasheet
#define RA8875_PLLC2_DIV64 0x06  ///< See datasheet
#define RA8875_PLLC2_DIV128 0x07 ///< See datasheet

#define RA8875_SYSR 0x10       ///< See datasheet
#define RA8875_SYSR_8BPP 0x00  ///< See datasheet
#define RA8875_SYSR_16BPP 0x0C ///< See datasheet
#define RA8875_SYSR_MCU8 0x00  ///< See datasheet
#define RA8875_SYSR_MCU16 0x03 ///< See datasheet

#define RA8875_PCSR 0x04       ///< See datasheet
#define RA8875_PCSR_PDATR 0x00 ///< See datasheet
#define RA8875_PCSR_PDATL 0x80 ///< See datasheet
#define RA8875_PCSR_CLK 0x00   ///< See datasheet
#define RA8875_PCSR_2CLK 0x01  ///< See datasheet
#define RA8875_PCSR_4CLK 0x02  ///< See datasheet
#define RA8875_PCSR_8CLK 0x03  ///< See datasheet

#define RA8875_HDWR 0x14 ///< See datasheet

#define RA8875_HNDFTR 0x15         ///< See datasheet
#define RA8875_HNDFTR_DE_HIGH 0x00 ///< See datasheet
#define RA8875_HNDFTR_DE_LOW 0x80  ///< See datasheet

#define RA8875_HNDR 0x16      ///< See datasheet
#define RA8875_HSTR 0x17      ///< See datasheet
#define RA8875_HPWR 0x18      ///< See datasheet
#define RA8875_HPWR_LOW 0x00  ///< See datasheet
#define RA8875_HPWR_HIGH 0x80 ///< See datasheet

#define RA8875_VDHR0 0x19     ///< See datasheet
#define RA8875_VDHR1 0x1A     ///< See datasheet
#define RA8875_VNDR0 0x1B     ///< See datasheet
#define RA8875_VNDR1 0x1C     ///< See datasheet
#define RA8875_VSTR0 0x1D     ///< See datasheet
#define RA8875_VSTR1 0x1E     ///< See datasheet
#define RA8875_VPWR 0x1F      ///< See datasheet
#define RA8875_VPWR_LOW 0x00  ///< See datasheet
#define RA8875_VPWR_HIGH 0x80 ///< See datasheet

#define RA8875_HSAW0 0x30 ///< See datasheet
#define RA8875_HSAW1 0x31 ///< See datasheet
#define RA8875_VSAW0 0x32 ///< See datasheet
#define RA8875_VSAW1 0x33 ///< See datasheet

#define RA8875_HEAW0 0x34 ///< See datasheet
#define RA8875_HEAW1 0x35 ///< See datasheet
#define RA8875_VEAW0 0x36 ///< See datasheet
#define RA8875_VEAW1 0x37 ///< See datasheet

#define RA8875_MCLR 0x8E            ///< See datasheet
#define RA8875_MCLR_START 0x80      ///< See datasheet
#define RA8875_MCLR_STOP 0x00       ///< See datasheet
#define RA8875_MCLR_READSTATUS 0x80 ///< See datasheet
#define RA8875_MCLR_FULL 0x00       ///< See datasheet
#define RA8875_MCLR_ACTIVE 0x40     ///< See datasheet

#define RA8875_DCR 0x90                   ///< See datasheet
#define RA8875_DCR_LINESQUTRI_START 0x80  ///< See datasheet
#define RA8875_DCR_LINESQUTRI_STOP 0x00   ///< See datasheet
#define RA8875_DCR_LINESQUTRI_STATUS 0x80 ///< See datasheet
#define RA8875_DCR_CIRCLE_START 0x40      ///< See datasheet
#define RA8875_DCR_CIRCLE_STATUS 0x40     ///< See datasheet
#define RA8875_DCR_CIRCLE_STOP 0x00       ///< See datasheet
#define RA8875_DCR_FILL 0x20              ///< See datasheet
#define RA8875_DCR_NOFILL 0x00            ///< See datasheet
#define RA8875_DCR_DRAWLINE 0x00          ///< See datasheet
#define RA8875_DCR_DRAWTRIANGLE 0x01      ///< See datasheet
#define RA8875_DCR_DRAWSQUARE 0x10        ///< See datasheet

#define RA8875_ELLIPSE 0xA0        ///< See datasheet
#define RA8875_ELLIPSE_STATUS 0x80 ///< See datasheet

#define RA8875_MWCR0 0x40         ///< See datasheet
#define RA8875_MWCR0_GFXMODE 0x00 ///< See datasheet
#define RA8875_MWCR0_TXTMODE 0x80 ///< See datasheet
#define RA8875_MWCR0_CURSOR 0x40  ///< See datasheet
#define RA8875_MWCR0_BLINK 0x20   ///< See datasheet

#define RA8875_MWCR0_DIRMASK 0x0C ///< Bitmask for Write Direction
#define RA8875_MWCR0_LRTD 0x00    ///< Left->Right then Top->Down
#define RA8875_MWCR0_RLTD 0x04    ///< Right->Left then Top->Down
#define RA8875_MWCR0_TDLR 0x08    ///< Top->Down then Left->Right
#define RA8875_MWCR0_DTLR 0x0C    ///< Down->Top then Left->Right

#define RA8875_BTCR 0x44  ///< See datasheet
#define RA8875_CURH0 0x46 ///< See datasheet
#define RA8875_CURH1 0x47 ///< See datasheet
#define RA8875_CURV0 0x48 ///< See datasheet
#define RA8875_CURV1 0x49 ///< See datasheet

#define RA8875_P1CR 0x8A         ///< See datasheet
#define RA8875_P1CR_ENABLE 0x80  ///< See datasheet
#define RA8875_P1CR_DISABLE 0x00 ///< See datasheet
#define RA8875_P1CR_CLKOUT 0x10  ///< See datasheet
#define RA8875_P1CR_PWMOUT 0x00  ///< See datasheet

#define RA8875_P1DCR 0x8B ///< See datasheet

#define RA8875_P2CR 0x8C         ///< See datasheet
#define RA8875_P2CR_ENABLE 0x80  ///< See datasheet
#define RA8875_P2CR_DISABLE 0x00 ///< See datasheet
#define RA8875_P2CR_CLKOUT 0x10  ///< See datasheet
#define RA8875_P2CR_PWMOUT 0x00  ///< See datasheet

#define RA8875_P2DCR 0x8D ///< See datasheet

#define RA8875_PWM_CLK_DIV1 0x00     ///< See datasheet
#define RA8875_PWM_CLK_DIV2 0x01     ///< See datasheet
#define RA8875_PWM_CLK_DIV4 0x02     ///< See datasheet
#define RA8875_PWM_CLK_DIV8 0x03     ///< See datasheet
#define RA8875_PWM_CLK_DIV16 0x04    ///< See datasheet
#define RA8875_PWM_CLK_DIV32 0x05    ///< See datasheet
#define RA8875_PWM_CLK_DIV64 0x06    ///< See datasheet
#define RA8875_PWM_CLK_DIV128 0x07   ///< See datasheet
#define RA8875_PWM_CLK_DIV256 0x08   ///< See datasheet
#define RA8875_PWM_CLK_DIV512 0x09   ///< See datasheet
#define RA8875_PWM_CLK_DIV1024 0x0A  ///< See datasheet
#define RA8875_PWM_CLK_DIV2048 0x0B  ///< See datasheet
#define RA8875_PWM_CLK_DIV4096 0x0C  ///< See datasheet
#define RA8875_PWM_CLK_DIV8192 0x0D  ///< See datasheet
#define RA8875_PWM_CLK_DIV16384 0x0E ///< See datasheet
#define RA8875_PWM_CLK_DIV32768 0x0F ///< See datasheet

#define RA8875_TPCR0 0x70               ///< See datasheet
#define RA8875_TPCR0_ENABLE 0x80        ///< See datasheet
#define RA8875_TPCR0_DISABLE 0x00       ///< See datasheet
#define RA8875_TPCR0_WAIT_512CLK 0x00   ///< See datasheet
#define RA8875_TPCR0_WAIT_1024CLK 0x10  ///< See datasheet
#define RA8875_TPCR0_WAIT_2048CLK 0x20  ///< See datasheet
#define RA8875_TPCR0_WAIT_4096CLK 0x30  ///< See datasheet
#define RA8875_TPCR0_WAIT_8192CLK 0x40  ///< See datasheet
#define RA8875_TPCR0_WAIT_16384CLK 0x50 ///< See datasheet
#define RA8875_TPCR0_WAIT_32768CLK 0x60 ///< See datasheet
#define RA8875_TPCR0_WAIT_65536CLK 0x70 ///< See datasheet
#define RA8875_TPCR0_WAKEENABLE 0x08    ///< See datasheet
#define RA8875_TPCR0_WAKEDISABLE 0x00   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV1 0x00   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV2 0x01   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV4 0x02   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV8 0x03   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV16 0x04  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV32 0x05  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV64 0x06  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV128 0x07 ///< See datasheet

#define RA8875_TPCR1 0x71            ///< See datasheet
#define RA8875_TPCR1_AUTO 0x00       ///< See datasheet
#define RA8875_TPCR1_MANUAL 0x40     ///< See datasheet
#define RA8875_TPCR1_VREFINT 0x00    ///< See datasheet
#define RA8875_TPCR1_VREFEXT 0x20    ///< See datasheet
#define RA8875_TPCR1_DEBOUNCE 0x04   ///< See datasheet
#define RA8875_TPCR1_NODEBOUNCE 0x00 ///< See datasheet
#define RA8875_TPCR1_IDLE 0x00       ///< See datasheet
#define RA8875_TPCR1_WAIT 0x01       ///< See datasheet
#define RA8875_TPCR1_LATCHX 0x02     ///< See datasheet
#define RA8875_TPCR1_LATCHY 0x03     ///< See datasheet

#define RA8875_TPXH 0x72  ///< See datasheet
#define RA8875_TPYH 0x73  ///< See datasheet
#define RA8875_TPXYL 0x74 ///< See datasheet

#define RA8875_INTC1 0xF0     ///< See datasheet
#define RA8875_INTC1_KEY 0x10 ///< See datasheet
#define RA8875_INTC1_DMA 0x08 ///< See datasheet
#define RA8875_INTC1_TP 0x04  ///< See datasheet
#define RA8875_INTC1_BTE 0x02 ///< See datasheet

#define RA8875_INTC2 0xF1     ///< See datasheet
#define RA8875_INTC2_KEY 0x10 ///< See datasheet
#define RA8875_INTC2_DMA 0x08 ///< See datasheet
#define RA8875_INTC2_TP 0x04  ///< See datasheet
#define RA8875_INTC2_BTE 0x02 ///< See datasheet

#define RA8875_SCROLL_BOTH 0x00   ///< See datasheet
#define RA8875_SCROLL_LAYER1 0x40 ///< See datasheet
#define RA8875_SCROLL_LAYER2 0x80 ///< See datasheet
#define RA8875_SCROLL_BUFFER 0xC0 ///< See datasheet

#endif /* RA8775_COMMANDS_H_ */
