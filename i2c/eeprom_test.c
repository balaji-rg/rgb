#include <stdio.h>
#include "eeprom_final.h"

#define SDA_PIN 6
#define SCL_PIN 7

int main()
{
	
	NEW_BUS_INIT(bus1, PORT_A, SDA_PIN, PORT_A, SCL_PIN);

        void *bus_id = i2c_bus_configure(&bus1);

        unsigned char data_word_address = 0x50;

        struct eeprom_operation_details write_details = {
                .operation = PAGE_WRITE,
                .total_bytes = 10
        };

        struct eeprom_operation_details read_details = {
                .operation = SEQUENTIAL,
                .total_bytes = 16
        };

        unsigned char arr1[16] = {0};
        unsigned char arr[16] = {0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x7a, 0x7b, 0x7c, 0x7d};

        printf("Writing the values \n");
        if (eeprom_write(bus_id, arr, data_word_address, &write_details))
                printf("failure");

	printf("Reading the values after writing\n");
        if (eeprom_read(bus_id, arr1, data_word_address, &read_details))
                printf("failure");

        for (unsigned char index = 0; index < 16; index++)
                printf("%x \n", arr1[index]);

        printf("Erasing the values \n");
        if(eeprom_erase(bus_id, data_word_address, &write_details))
                printf("failure");

        printf("Reading the values after erasing \n");
        if (eeprom_read(bus_id, arr1, data_word_address, &read_details))
                printf("failure");

        for (unsigned char index = 0; index < 16; index++)
                printf("%x \n", arr1[index]);

       return 0;
}

