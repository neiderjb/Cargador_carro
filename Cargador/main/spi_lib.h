
void sendtospi(bool ReadWrite, spi_device_handle_t spi, uint8_t command, uint8_t datatosend, uint8_t *dataread);
void send8(uint8_t d);
uint8_t read8(void);
void spidrawpixels(uint16_t *p, uint32_t num);
void spi_begin();
void spi_config();
