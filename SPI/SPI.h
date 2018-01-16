
#ifndef SPI_SPI_H_
#define SPI_SPI_H_

void spi_init(void);
uint8_t spi_send(uint8_t c);
void ISR();

#endif /* SPI_SPI_H_ */
