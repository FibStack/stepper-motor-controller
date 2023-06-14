/*
* Copyright (c) 2023, FibStack
* All rights reserved.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/


#ifndef TWI_H_
#define TWI_H_

#include <avr/eeprom.h>

#define RESPONSE_VERSION "FBSMC01_A001"
#define RESPONSE_INVALID "INVALID"
#define RESPONSE_OK "OK"
#define COMMAND_DELIMITER ":;,"
#define TWI_BUFFER_SIZE	150

extern uint8_t EEMEM eeprom_twi_address;

extern char read_buffer[];
extern char write_buffer[];
extern uint8_t bytes_read;
extern uint8_t bytes_written;


extern void TWI0_init(uint8_t address);
extern void TWI0_set_address(uint8_t address);
extern void TWI0_process_interrupt();
extern void TWI0_process_command();

extern void process_set_address();
extern void set_response(char *response);

#endif /* TWI_H_ */