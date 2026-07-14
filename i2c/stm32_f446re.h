#ifndef STM_32
#define STM_32

#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define PORT_D 3
#define PORT_H 7

enum mode{
	INPUT,
	OUTPUT,
	ALTERNATE_FUNCTION,
	ANALOG
};

enum type{
	PUSH_PULL,
	OPEN_DRAIN
};

enum pull_state{
	NO_PULLUP_PULLDOWN,
	PULL_UP,
	PULL_DOWN
};

enum op_state{
	LOW,
	HIGH,
};

void gpio_mode(unsigned char, unsigned char, enum mode);
void gpio_type(unsigned char, unsigned char, enum type);
void gpio_pull_state(unsigned char, unsigned char, enum pull_state);
void gpio_output_state(unsigned char, unsigned char, enum op_state);
void gpio_pin_toggle(unsigned char, unsigned char);
unsigned char gpio_read(unsigned char, unsigned char);

#endif
