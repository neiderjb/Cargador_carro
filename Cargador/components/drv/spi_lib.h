
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "driver/spi_master.h"

void preTransferCallback(spi_transaction_t *t);
void postTransferCallback(spi_transaction_t *t);

void send8(uint8_t d);
uint8_t read8(void);
void spidrawpixels(uint16_t *p, uint32_t num);
void spidrawpixels2(uint16_t *p, uint32_t num, uint8_t command);

void spi_begin();
void spi_config();
