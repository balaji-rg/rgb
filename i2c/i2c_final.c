#include <zephyr/kernel.h>
#include <stdlib.h>
#include "i2c_final.h"

#define MSB 7
#define ONE_BYTE 8
#define CURRENT_BIT(data, bits_sent)  (((data) >> (MSB - ((bits_sent) % ONE_BYTE))) & 1)
#define RECEIVED_BIT(bit, bits_received)  ((bit) << (MSB - ((bits_received) % ONE_BYTE)))

static void start_condition(struct i2c_bus_details *bus, unsigned char delay)
{
	gpio_output_state(bus->sda_port, bus->sda_pin, HIGH);
        gpio_output_state(bus->scl_port, bus->scl_pin, HIGH);
        gpio_output_state(bus->sda_port, bus->sda_pin, LOW);
	k_usleep(delay);
}

static char i2c_send(const unsigned char *data_buffer, const unsigned short byte_count, 
		const struct i2c_bus_details *i2c_bus_details,
		const unsigned char delay)
{
	unsigned short total_bits_sent = 0;

	while (total_bits_sent < byte_count * ONE_BYTE) {

		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, LOW);
		k_usleep(delay);
		gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, CURRENT_BIT(*data_buffer, total_bits_sent));
		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, HIGH);
		k_usleep(delay);

		if (!(++total_bits_sent % ONE_BYTE)) {   
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, LOW);    /* ack read */
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, HIGH);  /* release sda */
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, HIGH);
			k_usleep(delay);
			
			if (gpio_read(i2c_bus_details->sda_port, i2c_bus_details->sda_pin)) {
				return -FAILURE;
			}

			if (total_bits_sent == byte_count * ONE_BYTE) {
				return SUCCESS;
			}
			
			data_buffer++;

			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, LOW);
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, CURRENT_BIT(*data_buffer, total_bits_sent));
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, HIGH);
			k_usleep(delay);
			
			total_bits_sent++;

			/* check for clock stretching */
			while(!gpio_read(i2c_bus_details->scl_port, i2c_bus_details->scl_pin));

		}	
	}
	return -FAILURE;
}

static char i2c_receive(unsigned char *data_buffer,
		const unsigned short byte_count,
		const struct i2c_bus_details *i2c_bus_details,
		const unsigned char delay)
{
 	unsigned short total_bits_received = 0;

	while (total_bits_received < byte_count * ONE_BYTE) {
	
		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, LOW);
		k_usleep(delay);
		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, HIGH);
		k_usleep(delay);
		*data_buffer |= RECEIVED_BIT(gpio_read(i2c_bus_details->sda_port, i2c_bus_details->sda_pin), total_bits_received);
		
		if (!(++total_bits_received % ONE_BYTE)) {   
			
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, LOW);
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, 
							total_bits_received == (byte_count * ONE_BYTE));
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, HIGH);
			k_usleep(delay);
		
			/* release sda after nack */
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, LOW);
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, HIGH);

			data_buffer++;
		}
	}
	return SUCCESS;
}

static void stop_condition(struct i2c_bus_details *i2c_bus_details, unsigned char delay)
{
	gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, LOW);
	k_usleep(delay);

	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, LOW);

	gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, HIGH);
	k_usleep(delay);

	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, HIGH);
}

void *i2c_bus_configure(struct i2c_bus_details *i2c_bus_details)
{
	struct i2c_bus_details *new_bus;
	
	if (!i2c_bus_details)
		return NULL;

	new_bus = malloc(sizeof(struct i2c_bus_details));
	
	if (!new_bus)
		return NULL;

	*new_bus = *i2c_bus_details;
			
	gpio_output_type(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, OPEN_DRAIN);
	gpio_output_type(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, OPEN_DRAIN);

	gpio_pull_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PULL_UP);
	gpio_pull_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PULL_UP);

	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, HIGH);
	gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, HIGH);

	gpio_mode(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, OUTPUT);
	gpio_mode(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, OUTPUT);

	return new_bus;
}

char i2c_transfer(struct i2c_target_details *i2c_target_details,
		struct i2c_transfer_details *i2c_write_details,
		struct i2c_transfer_details *i2c_read_details)
{
	unsigned char result;
	unsigned char target_addr;
	unsigned char delay;
	struct i2c_bus_details *bus;

	if (!i2c_target_details ||
	    !(i2c_target_details->i2c_bus) ||
            i2c_target_details->target_address > 0x7f) {
		return -INVARG;
	}

	delay = ((1.0 / (KHZ(i2c_target_details->mode) * 2)) * 1000000);
	bus = i2c_target_details->i2c_bus;

	/* start condition */
	start_condition(bus, delay);

	if (!i2c_write_details && !i2c_read_details) {
		target_addr = ((i2c_target_details->target_address << 1) | I2C_WRITE);
		result = i2c_send(&target_addr, 1, bus, delay);
		goto stop;
	}

	if (i2c_write_details) {
		target_addr = ((i2c_target_details->target_address << 1) | I2C_WRITE);
		result = i2c_send(&target_addr, 1, bus, delay);
		
		if (result) {
			goto stop;
		}
		
		if (!i2c_write_details->data_buffer) {
			return -INVARG;
		}
		
		result = i2c_send(i2c_write_details->data_buffer,
					i2c_write_details->byte_count, bus, delay);	
	}

	if (i2c_read_details) {

		if (i2c_write_details) {
			start_condition(bus, delay);
		}

		target_addr = ((i2c_target_details->target_address << 1) | I2C_READ);
		result = i2c_send(&target_addr, 1, bus, delay);

		if (result) {
			goto stop;
		}
		
		if (!i2c_read_details->data_buffer) {
			return -INVARG;
		}

		result = i2c_receive(i2c_read_details->data_buffer,
					i2c_read_details->byte_count,  bus, delay);
	}
stop:
	stop_condition(bus,delay);
	return result;
}
