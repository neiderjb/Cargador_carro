#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "RA8875.h"
#include "spi_lib.h"
#include "gpio_lib.h"
#include "Parameters.h"
#include "driver/spi_master.h"

uint8_t readStatus(void)
{
    uint8_t x = 0;
    sendtospi(READ, spi_handle, RA8875_CMDREAD, 0, &x);
    return x;
}

void writeCommand(uint8_t d)
{
    sendtospi(WRITE, spi_handle, RA8875_CMDWRITE, d, NULL);
}

uint8_t readData(void)
{
    uint8_t x = 0;
    sendtospi(READ, spi_handle, RA8875_DATAREAD, 0, &x);
    return x;
}

void writeData(uint8_t d)
{
    sendtospi(WRITE, spi_handle, RA8875_DATAWRITE, d, NULL);
}

uint8_t readReg(uint8_t reg)
{
    writeCommand(reg);
    return readData();
}

void writeReg(uint8_t reg, uint8_t val)
{
    writeCommand(reg);
    writeData(val);
}

void PLLinit(void)
{
    if (_size == RA8875_480x80 || _size == RA8875_480x128 || _size == RA8875_480x272)
    {
        writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
        vTaskDelay(1);
        writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
        vTaskDelay(1);
    }
    else /* (_size == RA8875_800x480) */
    {
        writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 11);
        vTaskDelay(1);
        writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
        vTaskDelay(1);
    }
}

void initialize(void)
{
    PLLinit();
    writeReg(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

    /* Timing values */
    uint8_t pixclk;
    uint8_t hsync_start;
    uint8_t hsync_pw;
    uint8_t hsync_finetune;
    uint8_t hsync_nondisp;
    uint8_t vsync_pw;
    uint16_t vsync_nondisp;
    uint16_t vsync_start;

    /* Set the correct values for the display being used */
    if (_size == RA8875_480x80)
    {
        pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
        hsync_nondisp = 10;
        hsync_start = 8;
        hsync_pw = 48;
        hsync_finetune = 0;
        vsync_nondisp = 3;
        vsync_start = 8;
        vsync_pw = 10;
        _voffset = 192; // This uses the bottom 80 pixels of a 272 pixel controller
    }
    else if (_size == RA8875_480x128 || _size == RA8875_480x272)
    {
        pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
        hsync_nondisp = 10;
        hsync_start = 8;
        hsync_pw = 48;
        hsync_finetune = 0;
        vsync_nondisp = 3;
        vsync_start = 8;
        vsync_pw = 10;
        _voffset = 0;
    }
    else // (_size == RA8875_800x480)
    {
        pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
        hsync_nondisp = 26;
        hsync_start = 32;
        hsync_pw = 96;
        hsync_finetune = 0;
        vsync_nondisp = 32;
        vsync_start = 23;
        vsync_pw = 2;
        _voffset = 0;
    }

    writeReg(RA8875_PCSR, pixclk);
    vTaskDelay(1);
    /* Horizontal settings registers */
    writeReg(RA8875_HDWR, (_width / 8) - 1); // H width: (HDWR + 1) * 8 = 480
    writeReg(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
    writeReg(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2) / 8); // H non-display: HNDR * 8 + HNDFTR + 2 = 10
    writeReg(RA8875_HSTR, hsync_start / 8 - 1);                      // Hsync start: (HSTR + 1)*8
    writeReg(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw / 8 - 1));     // HSync pulse width = (HPWR+1) * 8

    /* Vertical settings registers */
    writeReg(RA8875_VDHR0, (uint16_t)(_height - 1 + _voffset) & 0xFF);
    writeReg(RA8875_VDHR1, (uint16_t)(_height - 1 + _voffset) >> 8);
    writeReg(RA8875_VNDR0, vsync_nondisp - 1); // V non-display period = VNDR + 1
    writeReg(RA8875_VNDR1, vsync_nondisp >> 8);
    writeReg(RA8875_VSTR0, vsync_start - 1); // Vsync start position = VSTR + 1
    writeReg(RA8875_VSTR1, vsync_start >> 8);
    writeReg(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1); // Vsync pulse width = VPWR + 1

    /* Set active window X */
    writeReg(RA8875_HSAW0, 0); // horizontal start point
    writeReg(RA8875_HSAW1, 0);
    writeReg(RA8875_HEAW0, (uint16_t)(_width - 1) & 0xFF); // horizontal end point
    writeReg(RA8875_HEAW1, (uint16_t)(_width - 1) >> 8);

    /* Set active window Y */
    writeReg(RA8875_VSAW0, 0 + _voffset); // vertical start point
    writeReg(RA8875_VSAW1, 0 + _voffset);
    writeReg(RA8875_VEAW0, (uint16_t)(_height - 1 + _voffset) & 0xFF); // vertical end point
    writeReg(RA8875_VEAW1, (uint16_t)(_height - 1 + _voffset) >> 8);

    /* ToDo: Setup touch panel? */

    /* Clear the entire window */
    writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
    vTaskDelay(100);
}

bool begin_RA8875(int reset, enum RA8875sizes s)
{

    _size = s;

    if (_size == RA8875_480x80)
    {
        _width = 480;
        _height = 80;
    }
    else if (_size == RA8875_480x128)
    {
        _width = 480;
        _height = 128;
    }
    else if (_size == RA8875_480x272)
    {
        _width = 480;
        _height = 272;
    }
    else if (_size == RA8875_800x480)
    {
        _width = 800;
        _height = 480;
    }
    else
    {
        return false;
    }
    _rotation = 0;

    //reset display
    gpio_begin(reset, 0);
    gpio_write(reset, 0);
    vTaskDelay(100);
    gpio_write(reset, 1);
    vTaskDelay(100);

    uint8_t x = readReg(0);
    if (x != 0x75)
    {
        printf("%x\n", x);
        return false;
    }

    initialize();

    return true;
}

void displayOn(bool on)
{
    if (on)
        writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
    else
        writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

void GPIOX(bool on)
{
    if (on)
        writeReg(RA8875_GPIOX, 1);
    else
        writeReg(RA8875_GPIOX, 0);
}

void PWM1config(bool on, uint8_t clock)
{
    if (on)
    {
        writeReg(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
    }
    else
    {
        writeReg(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
    }
}

void PWM1out(uint8_t p)
{
    writeReg(RA8875_P1DCR, p);
}

void fillScreen(uint16_t color)
{
    rectHelper(0, 0, _width - 1, _height - 1, color, true);
}

int16_t applyRotationX(int16_t x)
{
    switch (_rotation)
    {
    case 2:
        x = _width - 1 - x;
        break;
    }

    return x;
}

int16_t applyRotationY(int16_t y)
{
    switch (_rotation)
    {
    case 2:
        y = _height - 1 - y;
        break;
    }

    return y + _voffset;
}

bool waitPoll(uint8_t regname, uint8_t waitflag)
{
    /* Wait for the command to finish */
    while (1)
    {
        uint8_t temp = readReg(regname);
        if (!(temp & waitflag))
            return true;
        vTaskDelay(10);
    }
    return false; // MEMEFIX: yeah i know, unreached! - add timeout?
}

void rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled)
{
    x = applyRotationX(x);
    y = applyRotationY(y);
    w = applyRotationX(w);
    h = applyRotationY(h);

    /* Set X */
    writeCommand(0x91);
    writeData(x);
    writeCommand(0x92);
    writeData(x >> 8);

    /* Set Y */
    writeCommand(0x93);
    writeData(y);
    writeCommand(0x94);
    writeData(y >> 8);

    /* Set X1 */
    writeCommand(0x95);
    writeData(w);
    writeCommand(0x96);
    writeData((w) >> 8);

    /* Set Y1 */
    writeCommand(0x97);
    writeData(h);
    writeCommand(0x98);
    writeData((h) >> 8);

    /* Set Color */
    writeCommand(0x63);
    writeData((color & 0xf800) >> 11);
    writeCommand(0x64);
    writeData((color & 0x07e0) >> 5);
    writeCommand(0x65);
    writeData((color & 0x001f));

    /* Draw! */
    writeCommand(RA8875_DCR);
    if (filled)
    {
        writeData(0xB0);
    }
    else
    {
        writeData(0x90);
    }

    /* Wait for the command to finish */
    waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void touchEnable(bool on)
{
    uint8_t adcClk = (uint8_t)RA8875_TPCR0_ADCCLK_DIV4;

    if (_size == RA8875_800x480) //match up touch size with LCD size
        adcClk = (uint8_t)RA8875_TPCR0_ADCCLK_DIV16;

    if (on)
    {
        /* Enable Touch Panel (Reg 0x70) */
        writeReg(RA8875_TPCR0, RA8875_TPCR0_ENABLE |
                                   RA8875_TPCR0_WAIT_4096CLK |
                                   RA8875_TPCR0_WAKEENABLE |
                                   adcClk); // 10mhz max!
        /* Set Auto Mode      (Reg 0x71) */
        writeReg(RA8875_TPCR1, RA8875_TPCR1_AUTO |
                                   // RA8875_TPCR1_VREFEXT |
                                   RA8875_TPCR1_DEBOUNCE);
        /* Enable TP INT */
        writeReg(RA8875_INTC1, readReg(RA8875_INTC1) | RA8875_INTC1_TP);
    }
    else
    {
        /* Disable TP INT */
        writeReg(RA8875_INTC1, readReg(RA8875_INTC1) & ~RA8875_INTC1_TP);
        /* Disable Touch Panel (Reg 0x70) */
        writeReg(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
    }
}

uint16_t width(void)
{
    return _width;
}
uint16_t height(void)
{
    return _height;
}

bool touched(void)
{
    if (readReg(RA8875_INTC2) & RA8875_INTC2_TP)
        return true;
    return false;
}

bool touchRead(uint16_t *x, uint16_t *y)
{
    uint16_t tx, ty;
    uint8_t temp;

    tx = readReg(RA8875_TPXH);
    ty = readReg(RA8875_TPYH);
    temp = readReg(RA8875_TPXYL);
    tx <<= 2;
    ty <<= 2;
    tx |= temp & 0x03;        // get the bottom x bits
    ty |= (temp >> 2) & 0x03; // get the bottom y bits

    *x = tx;
    *y = ty;

    /* Clear TP INT Status */
    writeReg(RA8875_INTC2, RA8875_INTC2_TP);

    return true;
}
void circleHelper(int16_t x, int16_t y, int16_t r, uint16_t color, bool filled)
{
    x = applyRotationX(x);
    y = applyRotationY(y);

    /* Set X */
    writeCommand(0x99);
    writeData(x);
    writeCommand(0x9a);
    writeData(x >> 8);

    /* Set Y */
    writeCommand(0x9b);
    writeData(y);
    writeCommand(0x9c);
    writeData(y >> 8);

    /* Set Radius */
    writeCommand(0x9d);
    writeData(r);

    /* Set Color */
    writeCommand(0x63);
    writeData((color & 0xf800) >> 11);
    writeCommand(0x64);
    writeData((color & 0x07e0) >> 5);
    writeCommand(0x65);
    writeData((color & 0x001f));

    /* Draw! */
    writeCommand(RA8875_DCR);
    if (filled)
    {
        writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL);
    }
    else
    {
        writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
    }

    /* Wait for the command to finish */
    waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
}

void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
    circleHelper(x, y, r, color, true);
}