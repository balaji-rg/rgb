#include "stm32_f446re.h"
#ifndef I2C_H
#define I2C_H

#define I2C_WRITE	0
#define I2C_READ	1

#define SUCCESS 0
#define FAILURE 1
#define INVARG  2

#define STANDARD_MODE 100

#define NEW_BUS_INIT(bus_name, data_port, data_pin, clk_port, clk_pin)   \
	struct i2c_bus_details bus_name = { \
		.sda_pin = data_pin,        \
		.sda_port = data_port,      \
		.scl_pin = clk_pin,         \
		.scl_port = clk_port        \
	}                                   \


struct i2c_bus_details {
	unsigned char sda_pin;
	unsigned char sda_port;
	unsigned char scl_pin;
	unsigned char scl_port;
};

struct i2c_transfer_details {
	unsigned short byte_count;
	unsigned char *data_buffer;
};

struct i2c_target_details {
	unsigned char target_address;
	unsigned char mode;                         /* standard mode - 100khz */
        unsigned char i2c_bus;
};

/* api for user */

void *i2c_bus_configure(struct i2c_bus_details *i2c_bus_details);

char i2c_transfer(struct i2c_target_details *i2c_target_details, struct i2c_transfer_details *i2c_write_details,
		  struct i2c_transfer_details *i2c_read_details);
#endif
