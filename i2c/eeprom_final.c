#include "eeprom_final.h"
#include <stdlib.h>
#include <stdio.h>

#define EEPROM_ADDRESS 0x50

enum operation {
	WRITE,
	ERASE
};

static char eeprom_write_erase_api(void *bus_id, const unsigned char *data, unsigned char data_word_address, 
			  const struct eeprom_operation_details *eeprom_operation_details, enum operation mode)
{

	struct i2c_transfer_details transfer_details;
	struct i2c_target_details target_details;
	unsigned char *buffer;

	if  (!eeprom_operation_details || !bus_id) {
		return -INVARG;
	}
	
	transfer_details.byte_count = (eeprom_operation_details->total_bytes + 1);

	target_details.target_address = EEPROM_ADDRESS;
	target_details.mode = STANDARD_MODE;
	target_details.i2c_bus = (struct i2c_bus_details *)bus_id;
	
	buffer = malloc(eeprom_operation_details->total_bytes + 1);

	if (!buffer) {
		return -FAILURE;
	}
	*buffer = data_word_address;
	
	if (mode == WRITE) {
		for (short int bit = 0; bit < eeprom_operation_details->total_bytes; bit++) {
			*(buffer + (bit + 1)) = *(data + bit);
		}
	} else {
		for (short int bit = 1; bit <= eeprom_operation_details->total_bytes; bit++) {
			*(buffer + bit) = 0xff;
		}
	}
	
	transfer_details.data_buffer = buffer;
	
	if (i2c_transfer(&target_details, &transfer_details, NULL)) {
		free(buffer);
		return -FAILURE;
	}

	free(buffer);
	return SUCCESS;
}
char eeprom_write(void *bus_id, const unsigned char *data, unsigned char data_word_address, 
			  const struct eeprom_operation_details *eeprom_operation_details)
{
	if (!data) {
		return -INVARG;
	}

	return eeprom_write_erase_api(bus_id, data, data_word_address, eeprom_operation_details, WRITE);
}

char eeprom_erase(void *bus_id, unsigned char data_word_address, 
				const struct eeprom_operation_details *eeprom_operation_details)
{
	return eeprom_write_erase_api(bus_id, NULL, data_word_address, eeprom_operation_details, ERASE);
}

char eeprom_read(void *bus_id , unsigned char *data, unsigned char data_word_address,
			  const struct eeprom_operation_details *eeprom_operation_details)
{
	struct i2c_target_details target_details;
	struct i2c_transfer_details transfer_details;
	struct i2c_transfer_details data_addr;

	if (!data || !eeprom_operation_details || !bus_id) {
		return -INVARG;
	}

	target_details.target_address = EEPROM_ADDRESS;
	target_details.mode = STANDARD_MODE;
	target_details.i2c_bus = bus_id;

	transfer_details.byte_count = eeprom_operation_details->total_bytes;
	transfer_details.data_buffer = data;
	
	if (data_word_address){ 

		data_addr.byte_count = 1;
		data_addr.data_buffer = &data_word_address;
			
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
