#ifndef _GPIO_SPI_H
#define _GPIO_SPI_H

void SPISendByte(unsigned char val);
unsigned char SPIReceByte(void);
void SPIInit(void);

#endif /* _GPIO_SPI_H */
