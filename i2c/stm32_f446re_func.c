#include "stm32_f446re.h"

#define GPIO_BASE_ADDRESS    0x40020000

typedef struct {
         unsigned int mode_register;
         unsigned int type_register;
         unsigned int speed_register;
         unsigned int pullup_pulldown_register;
         unsigned int input_data_register;
         unsigned int output_data_register;
	 unsigned int bit_set_reset_register;
	 unsigned int lock_register;
	 unsigned int alternate_function_low_register;
	 unsigned int alternate_function_high_register;
}gpio;


void gpio_mode(unsigned char port_name, unsigned char pin, enum mode type)
{
	volatile gpio *port = (volatile gpio*)(GPIO_BASE_ADDRESS + (port_name * 0x400));
	port->mode_register &= (~(3 << (pin * 2)));
	port->mode_register |= (type << (pin * 2));
}

void gpio_output_state(unsigned char port_name, unsigned char pin, enum op_state state)
{
	volatile gpio *port = (volatile gpio*)(GPIO_BASE_ADDRESS + (port_name * 0x400));
	port->output_data_register &= (~(1 << pin));
	port->output_data_register |= (state << pin);
}

void gpio_pin_toggle(unsigned char port_name, unsigned char pin)
{
	volatile gpio *port = (volatile gpio*)(GPIO_BASE_ADDRESS + (port_name * 0x400));
	port->output_data_register ^= (1 << pin);
}

void gpio_type(unsigned char port_name, unsigned char pin, enum type pin_type)
{
	volatile gpio *port = (volatile gpio*)(GPIO_BASE_ADDRESS + (port_name * 0x400));
	if (pin_type)
		port->type_register |= (1 << pin);
	else
		port->type_register &= (~(1 << pin));
}

void gpio_pull_state(unsigned char port_name, unsigned char pin, enum pull_state state)
{
	volatile gpio *port = (volatile gpio*)(GPIO_BASE_ADDRESS + (port_name * 0x400));
	port->pullup_pulldown_register &= (~(3 << (pin * 2)));
	port->pullup_pulldown_register |= (state << (pin * 2));
}

unsigned char gpio_read(unsigned char port_name, unsigned char pin)
{
	volatile gpio *port = (volatile gpio*)(GPIO_BASE_ADDRESS + (port_name * 0x400));
	return ((port->input_data_register >> pin) & 1);
}

