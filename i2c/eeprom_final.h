#ifndef EEPROM_H
#define EEPROM_H
#include "i2c_final.h"

/* eeprom operations */
#define BYTE_WRITE 1
#define PAGE_WRITE 16

#define BYTE_ERASE 1
#define PAGE_ERASE 16

#define CURRENT_READ 0
#define RANDOM_READ  1
#define SEQUENTIAL   2

#ifndef I2C_H

#define NEW_BUS_INIT(bus_name, data_port, data_pin, clk_port, clk_pin)   \
	struct i2c_bus_details bus_name = { \
		.sda_pin = data_pin,        \
		.sda_port = data_port,      \
		.scl_pin = clk_pin,         \
		.scl_port = clk_port        \
	}                                   \

void *i2c_bus_configure(struct i2c_bus_details *i2c_bus_details);
#endif

struct eeprom_operation_details {
	unsigned char operation;
	unsigned short total_bytes;
};

char eeprom_write(void *bus_id, const unsigned char *data, unsigned char data_word_address,
		           const struct eeprom_operation_details *eeprom_operation_details);

char eeprom_read(void *bus_id, unsigned char *data, unsigned char data_word_address,
			  const struct eeprom_operation_details *eeprom_operation_details);

char eeprom_erase(void *bus_id, unsigned char data_word_address,
			   const struct eeprom_operation_details *eeprom_operation_details);

#endif
