#include "eeprom_final.h"
#include <stdlib.h>

#define EEPROM_ADDRESS 0x50
#define NULL ((void *)0)

char eeprom_write(void *bus_id, const unsigned char *data, unsigned char data_word_address, 
			  const struct eeprom_operation_details *eeprom_operation_details)
{
	if  (!data || !eeprom_operation_details || !bus_id) {
		return -INVARG;
	}
	
	struct i2c_transfer_details transfer_details = {
		.byte_count = (eeprom_operation_details->total_bytes + 1)
	};

	struct i2c_target_details target_details = {
		.target_address = EEPROM_ADDRESS,
		.mode = STANDARD_MODE,
		.i2c_bus = (struct i2c_bus_details *)bus_id
	};
	
	unsigned char *buffer = malloc(eeprom_operation_details->total_bytes + 1);

	if (!buffer) {
		return -FAILURE;
	}
	*buffer = data_word_address;

	for (short int bit = 0; bit < eeprom_operation_details->total_bytes; bit++) {
		*(buffer + (bit + 1)) = *(data + bit);
	}
	
	transfer_details.data_buffer = buffer;
	
	if (i2c_transfer(&target_details, &transfer_details, NULL)) {
		return -FAILURE;
	}

	return SUCCESS;
}

char eeprom_erase(void *bus_id, unsigned char data_word_address, 
				const struct eeprom_operation_details *eeprom_operation_details)
{
	if (!eeprom_operation_details || !bus_id) {
		return -INVARG;
	}
	
	struct i2c_transfer_details transfer_details = {
		.byte_count = eeprom_operation_details->total_bytes
	};

	struct i2c_target_details target_details = {
		.target_address = EEPROM_ADDRESS,
		.mode = STANDARD_MODE,
		.i2c_bus = (struct i2c_bus_details *)bus_id
	};

	unsigned char *buffer = malloc(eeprom_operation_details->total_bytes + 1);

	if (!buffer) {
		return -FAILURE;
	}
	*buffer = data_word_address;

	for (short int bit = 0; bit < eeprom_operation_details->total_bytes; bit++) {
		*(buffer + (bit + 1)) = 0xff;
	}
	
	transfer_details.data_buffer = buffer;
	
	if (i2c_transfer(&target_details, &transfer_details, NULL)) {
		return -FAILURE;
	}

	return SUCCESS;
}

char eeprom_read(void *bus_id , unsigned char *data, unsigned char data_word_address,
			  const struct eeprom_operation_details *eeprom_operation_details)
{
	if (!data || !eeprom_operation_details || !bus_id) {
		return -INVARG;
	}

	struct i2c_target_details target_details = {
		.target_address = EEPROM_ADDRESS,
		.mode = STANDARD_MODE,
		.i2c_bus = (struct i2c_bus_details *)bus_id
	};

	struct i2c_transfer_details transfer_details = {
		.byte_count = eeprom_operation_details->total_bytes,
		.data_buffer = data,
	};
	
	if (data_word_address){ 
	
		struct i2c_transfer_details data_addr = {
			.byte_count = 1,
			.data_buffer = &data_word_address,
		};
			
		if(i2c_transfer(&target_details, &data_addr, &transfer_details)) {
			return -FAILURE;
		}

	} else {
		if (i2c_transfer(&target_details, NULL, &transfer_details)) {
			return -FAILURE;
		}
	}
	
	return SUCCESS;
}	
