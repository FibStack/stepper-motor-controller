/*
* Copyright (c) 2023, FibStack
* All rights reserved.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "util.h"
#include "twi.h"
#include "tca.h"
#include "motors.h"

void PORTA_init();
void PORTC_init();

ISR(TWI0_TWIS_vect)
{
	// Processing receiving/sending data through I2C interface
	TWI0_process_interrupt();
}

ISR(TCA0_OVF_vect)
{
	// Processing commands at each TimerA overflow
	
	if(!is_switch_activated && !is_paused && move_device_id == MOTOR_DEVICES) {
		// No switch is activated, commands are not paused, and no override move command, run commands from the buffer
		run_command_on_device(&runCommnadsBufferA[head], 0);
		run_command_on_device(&runCommnadsBufferB[head], 1);
		run_command_on_device(&runCommnadsBufferC[head], 2);
		run_command_on_device(&runCommnadsBufferD[head], 3);
		
		if(!is_buffer_command_available(head)) {
			// No remaining steps, clear the other values from the buffer
			clear_command_struct(&runCommnadsBufferA[head]);
			clear_command_struct(&runCommnadsBufferB[head]);
			clear_command_struct(&runCommnadsBufferC[head]);
			clear_command_struct(&runCommnadsBufferD[head]);
			// Move to the next command if available
			uint8_t next_command_index = (head + 1) % COMMAND_BUFFER_SIZE;
			if(is_buffer_command_available(next_command_index)) {
				head = next_command_index;
			}
		}
	}
	else if (move_device_id < MOTOR_DEVICES && moveCommand.steps > 0)
	{
		// Execute move command, takes precedence over commands in the buffer
		run_command_on_device(&moveCommand, move_device_id);
	}
	else if (move_device_id < MOTOR_DEVICES && moveCommand.steps == 0)
	{
		// Reset move command
		clear_command_struct(&moveCommand);
		move_device_id = MOTOR_DEVICES;
		// Update if any switches are activated
		is_switch_activated = !((PORTB.IN >> 2) == 15);
		// Pause running commands, user must send resume command
		is_paused = 1;
	}
	
	// Stop if any of the limit switches are activated
	if((PORTB.IN >> 2) != 15)
	{
		is_switch_activated = 1;
	}
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

int main(void)
{
	uint8_t twi_address = eeprom_read_byte(&eeprom_twi_address);
	TWI0_init(twi_address);
	PORTA_init();
	PORTC_init();
	TCA0_init();
	clear_command_buffer();
	sei();
	
	while(1)
	{
		// Program loop
	}
}

void PORTC_init()
{
	// Configure physical pins 12, 13, 14, and 15 as output.
	PORTC.DIR |= PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm;
	PORTC.OUT |= PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm;
}

void PORTA_init()
{
	// Configure physical pins 2, 3, 4, and 5 as output.
	PORTA.DIR |= PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
	PORTA.OUT |= PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
}
