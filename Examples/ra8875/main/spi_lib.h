
void sendtospi(bool ReadWrite, spi_device_handle_t spi, uint8_t command, uint8_t datatosend, uint8_t *dataread);
void spi_begin();
void preTransferCallback(spi_transaction_t *t);
void postTransferCallback(spi_transaction_t *t);
void send8tospi(spi_device_handle_t spi, uint8_t datatosend);