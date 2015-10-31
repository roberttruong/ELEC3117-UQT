#include "SPI.h"

void pot_write(unsigned char data){
	spi_data(0);
	spi_data(data);	
}