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
  //sendtospi(READ, spi_handle, RA8875_CMDREAD, 0, &x);
  return x;
}

void writeCommand(uint8_t d)
{
  gpio_write(PIN_NUM_CS, 0);
  send8(RA8875_CMDWRITE);
  send8(d);
  gpio_write(PIN_NUM_CS, 1);
  //vTaskDelay(10 / portTICK_RATE_MS);
}

uint8_t readData(void)
{
#ifdef littleOpt
  gpio_set_level(PIN_NUM_CS, 0);
  vTaskDelay(10 / portTICK_RATE_MS);
  send8(RA8875_DATAREAD);
  uint8_t x = (uint8_t)read8();
  vTaskDelay(10 / portTICK_RATE_MS);
  gpio_set_level(PIN_NUM_CS, 1);
  vTaskDelay(10 / portTICK_RATE_MS);
  return x;
#else
  uint8_t read;
  gpio_set_level(PIN_NUM_CS, 0);
  send8(RA8875_DATAREAD);
  //read = (uint8_t)read8();
  return (uint8_t)read8();
  gpio_set_level(PIN_NUM_CS, 1);
#endif
}

void writeData(uint8_t d)
{
  gpio_write(PIN_NUM_CS, 0);
  send8(RA8875_DATAWRITE);
  send8(d);
  gpio_write(PIN_NUM_CS, 1);
  //vTaskDelay(10 / portTICK_RATE_MS);
}

uint8_t readRegRA(uint8_t reg)
{
#ifdef littleOpt
  gpio_set_level(PIN_NUM_CS, 0);
  vTaskDelay(10 / portTICK_RATE_MS);
  send8(RA8875_CMDWRITE);
  send8(reg);
  gpio_set_level(PIN_NUM_CS, 1);
  gpio_set_level(PIN_NUM_CS, 0);
  send8(RA8875_DATAREAD);
  uint8_t x = (uint8_t)read8();
  vTaskDelay(10 / portTICK_RATE_MS);
  gpio_set_level(PIN_NUM_CS, 1);
  return x; //readData();
#else
  writeCommand(reg);
  return readData();
#endif
}

void writeRegRA(uint8_t reg, uint8_t val)
{
  writeCommand(reg);
  writeData(val);
}

void PLLinit(void)
{
  if (_size == RA8875_480x80 || _size == RA8875_480x128 || _size == RA8875_480x272)
  {
    writeRegRA(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
    vTaskDelay(10 / portTICK_RATE_MS);
    writeRegRA(RA8875_PLLC2, RA8875_PLLC2_DIV4);
    vTaskDelay(10 / portTICK_RATE_MS);
  }
  else /* (_size == RA8875_800x480) */
  {
    printf("pll 800x480");
    writeRegRA(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 11);
    vTaskDelay(10 / portTICK_RATE_MS);
#ifdef littleOpt
    writeRegRA(RA8875_PLLC2, RA8875_PLLC2_DIV2);
#else
    writeRegRA(RA8875_PLLC2, RA8875_PLLC2_DIV1); //RA8875_PLLC2_DIV4
#endif

    vTaskDelay(10 / portTICK_RATE_MS);
  }
}

void initialize(void)
{

  PLLinit();
  writeRegRA(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

  /* Timing values */
  uint8_t pixclk;
  uint8_t hsync_start;
  uint8_t hsync_pw;
  uint8_t hsync_finetune;
  uint8_t hsync_nondisp;
  uint8_t vsync_pw;
  uint16_t vsync_nondisp;
  uint16_t vsync_start;

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

  _rotation = 0;

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

  writeRegRA(RA8875_PCSR, pixclk);
  vTaskDelay(10 / portTICK_RATE_MS);

  /* Horizontal settings registers */
  writeRegRA(RA8875_HDWR, (_width / 8) - 1); // H width: (HDWR + 1) * 8 = 480
  writeRegRA(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
  writeRegRA(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2) / 8); // H non-display: HNDR * 8 + HNDFTR + 2 = 10
  writeRegRA(RA8875_HSTR, hsync_start / 8 - 1);                      // Hsync start: (HSTR + 1)*8
  writeRegRA(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw / 8 - 1));     // HSync pulse width = (HPWR+1) * 8

  /* Vertical settings registers */
  writeRegRA(RA8875_VDHR0, (uint16_t)(_height - 1 + _voffset) & 0xFF);
  writeRegRA(RA8875_VDHR1, (uint16_t)(_height - 1 + _voffset) >> 8);
  writeRegRA(RA8875_VNDR0, vsync_nondisp - 1); // V non-display period = VNDR + 1
  writeRegRA(RA8875_VNDR1, vsync_nondisp >> 8);
  writeRegRA(RA8875_VSTR0, vsync_start - 1); // Vsync start position = VSTR + 1
  writeRegRA(RA8875_VSTR1, vsync_start >> 8);
  writeRegRA(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1); // Vsync pulse width = VPWR + 1

  /* Set active window X */
  writeRegRA(RA8875_HSAW0, 0); // horizontal start point
  writeRegRA(RA8875_HSAW1, 0);
  writeRegRA(RA8875_HEAW0, (uint16_t)(_width - 1) & 0xFF); // horizontal end point
  writeRegRA(RA8875_HEAW1, (uint16_t)(_width - 1) >> 8);

  /* Set active window Y */
  writeRegRA(RA8875_VSAW0, 0 + _voffset); // vertical start point
  writeRegRA(RA8875_VSAW1, 0 + _voffset);
  writeRegRA(RA8875_VEAW0, (uint16_t)(_height - 1 + _voffset) & 0xFF); // vertical end point
  writeRegRA(RA8875_VEAW1, (uint16_t)(_height - 1 + _voffset) >> 8);

  /* ToDo: Setup touch panel? */

  /* Clear the entire window */
  writeRegRA(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
  vTaskDelay(500 / portTICK_RATE_MS);
}

bool begin_RA8875(enum RA8875sizes s)
{
  _size = s;
  _textScale = 0;
  _rotation = 0;
  _voffset = 0;
  //reset display
  // gpio_begin(TOUCH_RESET, 0);
  // gpio_write(TOUCH_RESET, 0);
  gpio_begin(PIN_RESET_SCREEN, 0);
  gpio_begin(PIN_NUM_CS, 0);
  gpio_write(PIN_NUM_CS, 1);
  //Reset the display
  gpio_write(PIN_RESET_SCREEN, 0);
  vTaskDelay(100 / portTICK_RATE_MS);
  gpio_write(PIN_RESET_SCREEN, 1);
  vTaskDelay(500 / portTICK_RATE_MS);

  uint8_t x = readRegRA(0);
  if (x != 0x75)
  {
    ESP_LOGI("LOOK>", "reg 0x00 = %02X\n", x);
    return false;
  }

  initialize();

  spi_config(false);

  return true;
}

void displayOn(bool on)
{
  if (on)
    writeRegRA(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
  else
    writeRegRA(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

void GPIOX(bool on)
{
  if (on)
    writeRegRA(RA8875_GPIOX, 1);
  else
    writeRegRA(RA8875_GPIOX, 0);
}

void PWM1config(bool on, uint8_t clock)
{
  if (on)
  {
    writeRegRA(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
  }
  else
  {
    writeRegRA(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
  }
}

void PWM1out(uint8_t p)
{
  writeRegRA(RA8875_P1DCR, p);
}

void fillScreen(uint16_t color)
{
  printf("fillScreen!\n");
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
  //printf("enter waitPoll");
  while (1)
  {
    uint8_t temp = readRegRA(regname);
    if (!(temp & waitflag))
    {
      //printf("waitpoll temp: %d", temp);
      return true;
    }

    vTaskDelay(10 / portTICK_RATE_MS);
    //printf("waitpoll temp: %d", temp);
  }
  return false; // MEMEFIX: yeah i know, unreached! - add timeout?
                // vTaskDelay(100 / portTICK_RATE_MS);
  // return true;
}

void rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled)
{
  //printf("Enter fillScreen");
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
  printf("end fillScreen");
}

void touchEnable(bool on)
{
  uint8_t adcClk = (uint8_t)RA8875_TPCR0_ADCCLK_DIV4;

  if (_size == RA8875_800x480) //match up touch size with LCD size
    adcClk = (uint8_t)RA8875_TPCR0_ADCCLK_DIV16;

  if (on)
  {
    /* Enable Touch Panel (Reg 0x70) */
    writeRegRA(RA8875_TPCR0, RA8875_TPCR0_ENABLE |
                                 RA8875_TPCR0_WAIT_4096CLK |
                                 RA8875_TPCR0_WAKEENABLE |
                                 adcClk); // 10mhz max!
    /* Set Auto Mode      (Reg 0x71) */
    writeRegRA(RA8875_TPCR1, RA8875_TPCR1_AUTO |
                                 // RA8875_TPCR1_VREFEXT |
                                 RA8875_TPCR1_DEBOUNCE);
    /* Enable TP INT */
    writeRegRA(RA8875_INTC1, readRegRA(RA8875_INTC1) | RA8875_INTC1_TP);
  }
  else
  {
    /* Disable TP INT */
    writeRegRA(RA8875_INTC1, readRegRA(RA8875_INTC1) & ~RA8875_INTC1_TP);
    /* Disable Touch Panel (Reg 0x70) */
    writeRegRA(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
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
  if (readRegRA(RA8875_INTC2) & RA8875_INTC2_TP)
    return true;
  return false;
}

bool touchRead(uint16_t *x, uint16_t *y)
{
  uint16_t tx, ty;
  uint8_t temp;

  tx = readRegRA(RA8875_TPXH);
  ty = readRegRA(RA8875_TPYH);
  temp = readRegRA(RA8875_TPXYL);
  tx <<= 2;
  ty <<= 2;
  tx |= temp & 0x03;        // get the bottom x bits
  ty |= (temp >> 2) & 0x03; // get the bottom y bits

  *x = tx;
  *y = ty;

  /* Clear TP INT Status */
  writeRegRA(RA8875_INTC2, RA8875_INTC2_TP);

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
///////////////////////////////////
void textMode(void)
{
  /* Set text mode */
  writeCommand(RA8875_MWCR0);
  uint8_t temp = readData();
  temp |= RA8875_MWCR0_TXTMODE; // Set bit 7
  writeData(temp);

  /* Select the internal (ROM) font */
  writeCommand(0x21);
  temp = readData();
  temp &= ~((1 << 7) | (1 << 5)); // Clear bits 7 and 5
  writeData(temp);
}

void textSetCursor(uint16_t x, uint16_t y)
{
  x = applyRotationX(x);
  y = applyRotationY(y);

  /* Set cursor location */
  writeCommand(0x2A);
  writeData(x & 0xFF);
  writeCommand(0x2B);
  writeData(x >> 8);
  writeCommand(0x2C);
  writeData(y & 0xFF);
  writeCommand(0x2D);
  writeData(y >> 8);
}

void textColor(uint16_t foreColor, uint16_t bgColor)
{
  /* Set Fore Color */
  writeCommand(0x63);
  writeData((foreColor & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((foreColor & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((foreColor & 0x001f));

  /* Set Background Color */
  writeCommand(0x60);
  writeData((bgColor & 0xf800) >> 11);
  writeCommand(0x61);
  writeData((bgColor & 0x07e0) >> 5);
  writeCommand(0x62);
  writeData((bgColor & 0x001f));

  /* Clear transparency flag */
  writeCommand(0x22);
  uint8_t temp = readData();
  temp &= ~(1 << 6); // Clear bit 6
  writeData(temp);
}

void textTransparent(uint16_t foreColor)
{
  /* Set Fore Color */
  writeCommand(0x63);
  writeData((foreColor & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((foreColor & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((foreColor & 0x001f));

  /* Set transparency flag */
  writeCommand(0x22);
  uint8_t temp = readData();
  temp |= (1 << 6); // Set bit 6
  writeData(temp);
}

void textEnlarge(uint8_t scale)
{
  if (scale > 3)
    scale = 3; // highest setting is 3

  /* Set font size flags */
  writeCommand(0x22);
  uint8_t temp = readData();
  temp &= ~(0xF); // Clears bits 0..3
  temp |= scale << 2;
  temp |= scale;

  writeData(temp);

  _textScale = scale;
}

void cursorBlink(uint8_t rate)
{

  writeCommand(RA8875_MWCR0);
  uint8_t temp = readData();
  temp |= RA8875_MWCR0_CURSOR;
  writeData(temp);

  writeCommand(RA8875_MWCR0);
  temp = readData();
  temp |= RA8875_MWCR0_BLINK;
  writeData(temp);

  if (rate > 254)
    rate = 255;
  writeCommand(RA8875_BTCR);
  writeData(rate);
}

void textWrite(const char *buffer, uint16_t len)
{
  if (len == 0)
    len = strlen(buffer);
  writeCommand(RA8875_MRWC);
  for (uint16_t i = 0; i < len; i++)
  {
    writeData(buffer[i]);
#if defined(__arm__)
    // This delay is needed with textEnlarge(1) because
    // Teensy 3.X is much faster than Arduino Uno
    if (_textScale > 0)
      vTaskDelay(10);
#else
    // For others, delay starting with textEnlarge(2)
    if (_textScale > 1)
      vTaskDelay(10);
#endif
  }
}

void graphicsMode(void)
{
  writeCommand(RA8875_MWCR0);
  uint8_t temp = readData();
  temp &= ~RA8875_MWCR0_TXTMODE; // bit #7
  writeData(temp);
}

void setXY(uint16_t x, uint16_t y)
{
  writeRegRA(RA8875_CURH0, x);
  writeRegRA(RA8875_CURH1, x >> 8);
  writeRegRA(RA8875_CURV0, y);
  writeRegRA(RA8875_CURV1, y >> 8);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  rectHelper(x, y, x + w - 1, y + h - 1, color, true);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  x0 = applyRotationX(x0);
  y0 = applyRotationY(y0);
  x1 = applyRotationX(x1);
  y1 = applyRotationY(y1);

  /* Set X */
  writeCommand(0x91);
  writeData(x0);
  writeCommand(0x92);
  writeData(x0 >> 8);

  /* Set Y */
  writeCommand(0x93);
  writeData(y0);
  writeCommand(0x94);
  writeData(y0 >> 8);

  /* Set X1 */
  writeCommand(0x95);
  writeData(x1);
  writeCommand(0x96);
  writeData((x1) >> 8);

  /* Set Y1 */
  writeCommand(0x97);
  writeData(y1);
  writeCommand(0x98);
  writeData((y1) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  writeData(0x80);

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  drawLine(x, y, x, y + h, color);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  drawLine(x, y, x + w, y, color);
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  rectHelper(x, y, x + w - 1, y + h - 1, color, false);
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  triangleHelper(x0, y0, x1, y1, x2, y2, color, false);
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  triangleHelper(x0, y0, x1, y1, x2, y2, color, true);
}

void drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color)
{
  ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, false);
}

void fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color)
{
  ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, true);
}

void drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color)
{
  curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
}

void fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color)
{
  curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
}

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  roundRectHelper(x, y, x + w, y + h, r, color, false);
}

void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  roundRectHelper(x, y, x + w, y + h, r, color, true);
}

void triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled)
{
  x0 = applyRotationX(x0);
  y0 = applyRotationY(y0);
  x1 = applyRotationX(x1);
  y1 = applyRotationY(y1);
  x2 = applyRotationX(x2);
  y2 = applyRotationY(y2);

  /* Set Point 0 */
  writeCommand(0x91);
  writeData(x0);
  writeCommand(0x92);
  writeData(x0 >> 8);
  writeCommand(0x93);
  writeData(y0);
  writeCommand(0x94);
  writeData(y0 >> 8);

  /* Set Point 1 */
  writeCommand(0x95);
  writeData(x1);
  writeCommand(0x96);
  writeData(x1 >> 8);
  writeCommand(0x97);
  writeData(y1);
  writeCommand(0x98);
  writeData(y1 >> 8);

  /* Set Point 2 */
  writeCommand(0xA9);
  writeData(x2);
  writeCommand(0xAA);
  writeData(x2 >> 8);
  writeCommand(0xAB);
  writeData(y2);
  writeCommand(0xAC);
  writeData(y2 >> 8);

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
    writeData(0xA1);
  }
  else
  {
    writeData(0x81);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled)
{
  xCenter = applyRotationX(xCenter);
  yCenter = applyRotationY(yCenter);

  /* Set Center Point */
  writeCommand(0xA5);
  writeData(xCenter);
  writeCommand(0xA6);
  writeData(xCenter >> 8);
  writeCommand(0xA7);
  writeData(yCenter);
  writeCommand(0xA8);
  writeData(yCenter >> 8);

  /* Set Long and Short Axis */
  writeCommand(0xA1);
  writeData(longAxis);
  writeCommand(0xA2);
  writeData(longAxis >> 8);
  writeCommand(0xA3);
  writeData(shortAxis);
  writeCommand(0xA4);
  writeData(shortAxis >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(0xA0);
  if (filled)
  {
    writeData(0xC0);
  }
  else
  {
    writeData(0x80);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

void curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled)
{
  xCenter = applyRotationX(xCenter);
  yCenter = applyRotationY(yCenter);
  curvePart = (curvePart + _rotation) % 4;

  /* Set Center Point */
  writeCommand(0xA5);
  writeData(xCenter);
  writeCommand(0xA6);
  writeData(xCenter >> 8);
  writeCommand(0xA7);
  writeData(yCenter);
  writeCommand(0xA8);
  writeData(yCenter >> 8);

  /* Set Long and Short Axis */
  writeCommand(0xA1);
  writeData(longAxis);
  writeCommand(0xA2);
  writeData(longAxis >> 8);
  writeCommand(0xA3);
  writeData(shortAxis);
  writeCommand(0xA4);
  writeData(shortAxis >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(0xA0);
  if (filled)
  {
    writeData(0xD0 | (curvePart & 0x03));
  }
  else
  {
    writeData(0x90 | (curvePart & 0x03));
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

void roundRectHelper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled)
{
  x = applyRotationX(x);
  y = applyRotationY(y);
  w = applyRotationX(w);
  h = applyRotationY(h);
  //   if (x > w) swap(x, w);
  //   if (y > h) swap(y, h);

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

  writeCommand(0xA1);
  writeData(r);
  writeCommand(0xA2);
  writeData((r) >> 8);

  writeCommand(0xA3);
  writeData(r);
  writeCommand(0xA4);
  writeData((r) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_ELLIPSE);
  if (filled)
  {
    writeData(0xE0);
  }
  else
  {
    writeData(0xA0);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_ELLIPSE, RA8875_DCR_LINESQUTRI_STATUS);
}

void setScrollWindow(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t mode)
{
  // Horizontal Start point of Scroll Window
  writeCommand(0x38);
  writeData(x);
  writeCommand(0x39);
  writeData(x >> 8);

  // Vertical Start Point of Scroll Window
  writeCommand(0x3a);
  writeData(y);
  writeCommand(0x3b);
  writeData(y >> 8);

  // Horizontal End Point of Scroll Window
  writeCommand(0x3c);
  writeData(x + w);
  writeCommand(0x3d);
  writeData((x + w) >> 8);

  // Vertical End Point of Scroll Window
  writeCommand(0x3e);
  writeData(y + h);
  writeCommand(0x3f);
  writeData((y + h) >> 8);

  // Scroll function setting
  writeCommand(0x52);
  writeData(mode);
}

void scrollX(int16_t dist)
{
  writeCommand(0x24);
  writeData(dist);
  writeCommand(0x25);
  writeData(dist >> 8);
}

void scrollY(int16_t dist)
{
  writeCommand(0x26);
  writeData(dist);
  writeCommand(0x27);
  writeData(dist >> 8);
}

void drawPixels(uint16_t *p, uint32_t num, int16_t x, int16_t y)
{
  x = applyRotationX(x);
  y = applyRotationY(y);

  writeRegRA(RA8875_CURH0, x);
  writeRegRA(RA8875_CURH1, x >> 8);
  writeRegRA(RA8875_CURV0, y);
  writeRegRA(RA8875_CURV1, y >> 8);

  uint8_t dir = RA8875_MWCR0_LRTD;
  if (_rotation == 2)
  {
    dir = RA8875_MWCR0_RLTD;
  }
  writeRegRA(RA8875_MWCR0, (readRegRA(RA8875_MWCR0) & ~RA8875_MWCR0_DIRMASK) | dir);

  writeCommand(RA8875_MRWC);
  gpio_set_level(PIN_NUM_CS, 0);
  //spidrawpixels(p, num);
  spidrawpixels2(p, num, RA8875_DATAWRITE);
  gpio_set_level(PIN_NUM_CS, 1);
}

void drawPixels2(uint16_t *p, uint32_t num, int16_t x, int16_t y, int16_t x2, int16_t y2)
{
  x = applyRotationX(x);
  y = applyRotationY(y);

  writeRegRA(RA8875_CURH0, x);
  writeRegRA(RA8875_CURH1, x >> 8);
  writeRegRA(RA8875_CURV0, y);
  writeRegRA(RA8875_CURV1, y >> 8);

  writeRegRA(RA8875_HSAW0, x);
  writeRegRA(RA8875_HSAW1, x >> 8);
  writeRegRA(RA8875_VSAW0, y);
  writeRegRA(RA8875_VSAW1, y >> 8);

  writeRegRA(RA8875_HEAW0, x2);
  writeRegRA(RA8875_HEAW1, x2 >> 8);
  writeRegRA(RA8875_VEAW0, y2);
  writeRegRA(RA8875_VEAW1, y2 >> 8);

  writeRegRA(RA8875_MWCR0, 0x00); //(readReg(RA8875_MWCR0) & ~RA8875_MWCR0_DIRMASK) | dir

  writeCommand(RA8875_MRWC);
  gpio_set_level(PIN_NUM_CS, 0);
  spidrawpixelsNew(p, num, RA8875_DATAWRITE);
  gpio_set_level(PIN_NUM_CS, 1);
}

void init_screen()
{
  printf("Found RA8875, Display initialized!\n");
  displayOn(true);
  //printf("Display ON!\n");
  GPIOX(true); // Enable TFT - display enable tied to GPIOX
  //printf("GPIOX!\n");
  PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  //printf("PWM1config!\n");
  PWM1out(255);
  //printf("PWM1out!\n");
  vTaskDelay(100 / portTICK_RATE_MS);
  fillScreen(RA8875_WHITE);
  textMode();
  textColor(RA8875_BLACK,RA8875_WHITE);
  textSetCursor(100,100);
  textWrite("INICIANDO... Espere Por Favor!",strlen("INICIANDO... Espere Por Favor!"));
#ifndef littleOpt
  fillScreen(RA8875_WHITE);
  printf("fillScreen White \n");
  vTaskDelay(100 / portTICK_RATE_MS);
#endif
  printf("End Display initialized!\n");
}
