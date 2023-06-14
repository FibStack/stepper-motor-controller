/*
* Copyright (c) 2023, FibStack
* All rights reserved.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "twi.h"
#include "util.h"
#include "motors.h"

uint8_t EEMEM eeprom_twi_address = 0x50; // Default board I2C Slave Address

char read_buffer[TWI_BUFFER_SIZE];
char write_buffer[TWI_BUFFER_SIZE];

uint8_t bytes_read = 0;
uint8_t bytes_written = 0;

// Stores validation error codes when validating commands
uint8_t error_validation_code = 0;

/**
* Initializes the TWI0 peripheral
*/
void TWI0_init(uint8_t address)
{
	TWI0.SADDR = address << 1; // Set the slave address
	TWI0.SCTRLA = TWI_DIEN_bm // Data Interrupt Enable
	| TWI_APIEN_bm // Address or Stop Interrupt Enable
	| TWI_PIEN_bm // Enable Stop Interrupt
	| TWI_ENABLE_bm; // TWI0 Module Enable
	
	// Initialize buffers with 0
	memset(read_buffer, '\0', TWI_BUFFER_SIZE);
	memset(write_buffer, '\0', TWI_BUFFER_SIZE);
}

/**
* Sets the I2C slave address
*/
void TWI0_set_address(uint8_t address)
{
	TWI0.SADDR = address << 1;
}

/**
* Processes receiving and sending data
*/
void TWI0_process_interrupt()
{
	if(TWI0.SSTATUS & TWI_APIF_bm) // Address match/stop interrupt
	{
		if(TWI0.SSTATUS & TWI_COLL_bm)
		{
			TWI0.SSTATUS |= TWI_COLL_bm; // Clear Collision flag
			TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc; // Complete transaction
			return;
		}
		
		if(TWI0.SSTATUS & TWI_AP_bm)
		{
			TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc; // send ACK after address match
		}
		else
		{
			TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc; // complete transaction after Stop
			bytes_read = 0;
			bytes_written = 0;
		}
	}
	
	if(TWI0.SSTATUS & TWI_DIF_bm) // data interrupt
	{
		if(TWI0.SSTATUS & TWI_DIR_bm)
		{
			// Transmit data to Master
			if(bytes_written < TWI_BUFFER_SIZE)
			{
				uint8_t data = write_buffer[bytes_written];
				TWI0.SDATA = data;
				bytes_written++;
				TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc; // Send ACK, wait for another interrupt
			}
			else
			{
				TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;  // Transaction complete
			}
		}
		else
		{
			// Receive data from Master
			if(bytes_read < TWI_BUFFER_SIZE)
			{
				uint8_t data = TWI0.SDATA;
				read_buffer[bytes_read] = data;
				if(data == 0x00) {
					TWI0_process_command();
					bytes_read = 0;
				}
				bytes_read++;
				TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc; // Send ACK, data received, wait for another interrupt
			}
			else
			{
				TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;  // Transaction complete
			}
			
		}
	}
}

/**
* Writes a string to the write buffer.
*/
void set_response(char *response)
{
	memset(write_buffer, '\0', TWI_BUFFER_SIZE); // clear the buffer
	strcpy(write_buffer, response); // set new text
}


/**
* Processes the run command.
*/
void process_run()
{
	error_validation_code = 0; // Reset error code
	
	// Check if buffer available
	if(tail == head && (runCommnadsBufferA->steps > 0 || runCommnadsBufferB->steps > 0 || runCommnadsBufferC->steps > 0 || runCommnadsBufferD->steps > 0)) {
		error_validation_code = 1; // Buffer is full
		return;
	}
	
	// Get first part of command with device id, direction and steps
	char *command_value = strtok(NULL, COMMAND_DELIMITER);
	while(command_value != NULL)
	{
		RunCommand* run_command;
		// Get command buffer location based on device id specified
		switch(command_value[0])
		{
			case 'A':
			case 'a': run_command = &runCommnadsBufferA[tail]; break;
			case 'B':
			case 'b': run_command = &runCommnadsBufferB[tail]; break;
			case 'C':
			case 'c': run_command = &runCommnadsBufferC[tail]; break;
			case 'D':
			case 'd': run_command = &runCommnadsBufferD[tail]; break;
			default: error_validation_code = 2; return;
		}
		
		// Rotation direction, 1 - clockwise, 0 - counter clockwise
		run_command->dir = command_value[1] == '-' ? 0 : 1;
		uint8_t step_value_index_start = command_value[1] == '-' ? 2 : 1;
		
		// Get steps value
		run_command->steps = str2ulong(&command_value[step_value_index_start]);
		if(num_conversion_error != 0)
		{
			error_validation_code = 3; // Invalid steps value
			break;
		}
		
		// Get speed value
		char *speed_value = strtok(NULL, COMMAND_DELIMITER);
		run_command->speed = str2num(speed_value);
		if(num_conversion_error != 0)
		{
			error_validation_code = 4; // Invalid speed value
			break;
		}
		run_command->counter = 0;
		
		// Read next command value
		command_value = strtok(NULL, COMMAND_DELIMITER);
	}
	
	// Move tail pointer only when there is no error
	if(error_validation_code == 0) {
		tail = (tail + 1) % COMMAND_BUFFER_SIZE;
	}

}

/**
* Processes the move command.
*/
void process_move()
{
	error_validation_code = 0; // Reset error code
	
	// Get move command part with device id, direction and steps
	char *command_value = strtok(NULL, COMMAND_DELIMITER);
	if(command_value != NULL)
	{
		switch(command_value[0])
		{
			case 'A':
			case 'a': move_device_id = 0; break;
			case 'B':
			case 'b': move_device_id = 1; break;
			case 'C':
			case 'c': move_device_id = 2; break;
			case 'D':
			case 'd': move_device_id = 3; break;
			default: error_validation_code = 2; return;
		}

		
		// Rotation direction, 1 - clockwise, 0 - counter clockwise
		moveCommand.dir = command_value[1] == '-' ? 0 : 1;
		uint8_t step_value_index_start = command_value[1] == '-' ? 2 : 1;
		
		// Get steps value
		moveCommand.steps = str2ulong(&command_value[step_value_index_start]);
		if(num_conversion_error != 0)
		{
			error_validation_code = 3; // Invalid steps value
			return;
		}
		
		// Get speed value
		char *speed_value = strtok(NULL, COMMAND_DELIMITER);
		moveCommand.speed = str2num(speed_value);
		if(num_conversion_error != 0)
		{
			error_validation_code = 4; // Invalid speed value
			return;
		}
	}
	else
	{
		error_validation_code = 2; // Invalid command
	}
	
}

/**
* Attaches the command values to the status string.
*/
void attachCommand(char* status, RunCommand* run_command, uint8_t device_id)
{
	char device[2] = {'A', '\0'};
	device[0] = device_id + 'A';
	strcat(status, "\n");
	strcat(status, device); // Device id
	strcat(status, ":");
	if(!run_command->dir)
	{
		strcat(status, "-"); // direction
	}
	char *steps = num2str(run_command->steps);
	char *speed = num2str(run_command->speed);
	strcat(status, steps); // Steps
	strcat(status, ",");
	strcat(status, speed); // Speed
	free(steps);
	free(speed);
}

/**
* Processes the status command.
*/
void process_status()
{
	char *status = malloc(sizeof(char)*TWI_BUFFER_SIZE);
	
	if(move_device_id < MOTOR_DEVICES)
	{
		// Moving status
		strcat(status, "\nMOVE");
		attachCommand(status, &moveCommand, move_device_id);
	}
	else
	{
		// Run commands status
		if(is_paused)
		{
			strcat(status, "\nPAUSED");
		}
		else
		{
			strcat(status, "\nRUN");
		}
		attachCommand(status, &runCommnadsBufferA[head], 0);
		attachCommand(status, &runCommnadsBufferB[head], 1);
		attachCommand(status, &runCommnadsBufferC[head], 2);
		attachCommand(status, &runCommnadsBufferD[head], 3);
	}
	
	// Limit switches status
	strcat(status, "\nSW:");
	switch(PORTB.IN >> 2) {
		case 15: strcat(status, "NONE"); break;
		case 14: strcat(status, "SW0"); break;
		case 6: strcat(status, "SW1"); break;
		case 10: strcat(status, "SW2"); break;
		case 2: strcat(status, "SW3"); break;
		case 12: strcat(status, "SW4"); break;
		case 4: strcat(status, "SW5"); break;
		case 8: strcat(status, "SW6"); break;
		case 0: strcat(status, "SW5"); break;
		default: strcat(status, "UNDF"); break;
	}
	
	// Buffer status
	char *buffer_size = num2str(COMMAND_BUFFER_SIZE); // Total buffer size
	
	// Compute total commands currently stored in the buffer
	int16_t commands = tail - head;
	if(commands < 0) {
		// negative, means tail index is before head index
		commands += COMMAND_BUFFER_SIZE;
	}
	else if (commands == 0 && is_buffer_command_available(head))
	{
		// case when buffer is full
		commands = COMMAND_BUFFER_SIZE;
	}
	else if (commands == 1 && !is_buffer_command_available(head))
	{
		// case when head and tail index are next to each other but no commands running
		commands = 0;
	}
	
	char *commands_in_buffer = num2str(commands);
	strcat(status, "\nBUFF:");
	strcat(status,commands_in_buffer);
	strcat(status, "/");
	strcat(status, buffer_size);
	
	set_response(status);
	
	// Clean up
	free(commands_in_buffer);
	free(buffer_size);
	free(status);
}

/**
* Writes error response based on error code
*/
void set_error_response()
{
	switch(error_validation_code)
	{
		case 1: set_response("BUFFER FULL"); break;
		case 2: set_response("INVALID DEVICE ID"); break;
		case 3: set_response("INVALID STEPS VALUE"); break;
		case 4: set_response("INVALID SPEED VALUE"); break;
		default: break;
	}
}

/**
* Processes the commands received from the master
*/
void TWI0_process_command()
{
	if(bytes_read > 0)
	{
		char *token = strtok(read_buffer, ":");
		
		
		if(strcmp("version", token) == 0)
		{
			// function: version
			// Returns the device version
			// Format: version
			set_response(RESPONSE_VERSION);
		}
		else if(strcmp("setaddr", token) == 0)
		{
			// function: setaddr
			// Sets the board I2C address
			// Format: setaddr:<new_address_value>
			process_set_address();
		}
		else if (strcmp("status", token) == 0)
		{
			// function: status
			// Shows current status of the board
			// Format: status
			process_status();
		}
		else if (strcmp("run", token) == 0)
		{
			// function: run
			// Add command to the buffer
			// Format: run:<device_id[A,B,C, or D]>:<steps[+/- 32-bit integer]>,<speed[+16-bit integer]>:...
			// If user specifies multiple commands for the same device, the last one will override previous values
			error_validation_code = 0; // reset the error code
			process_run();
			if (error_validation_code > 0)
			{
				// Clear any changes in the buffer, unless overflow
				if(error_validation_code > 1)
				{
					clear_command_struct(&runCommnadsBufferA[tail]);
					clear_command_struct(&runCommnadsBufferB[tail]);
					clear_command_struct(&runCommnadsBufferC[tail]);
					clear_command_struct(&runCommnadsBufferD[tail]);
				}
				set_error_response();
			}
			else
			{
				set_response(RESPONSE_OK);
			}
		}
		else if (strcmp("move", token) == 0)
		{
			// function: move
			// Moves only the specified motor, this command will pause other commands and reset the limit switch
			// Will move only the first specified motor, others will be ignored
			// Format: move:<device_id[A,B,C, or D]>:<steps[+/- 32-bit integer]>,<speed[+16-bit integer]>
			error_validation_code = 0; // reset the error code
			process_move();
			if (error_validation_code > 0)
			{
				clear_command_struct(&moveCommand);
				move_device_id = MOTOR_DEVICES;
				set_error_response();
			}
			else
			{
				set_response(RESPONSE_OK);
			}
		}
		else if (strcmp("pause", token) == 0)
		{
			// function: pause
			// Pause all commands, no effect if already paused
			// Format: pause
			is_paused = 1;
			set_response(RESPONSE_OK);
		}
		else if (strcmp("resume", token) == 0)
		{
			// function: resume
			// Resume paused commands, no effect if already running
			// Format: resume
			is_paused = 0;
			set_response(RESPONSE_OK);
		}
		else if (strcmp("reset", token) == 0)
		{
			// function: reset
			// Clears the command buffer and cancels the move command
			// Format: reset
			clear_command_struct(&moveCommand);
			move_device_id = MOTOR_DEVICES;
			clear_command_buffer();
			is_paused = 0;
			set_response(RESPONSE_OK);
			head = 0;
			tail = 0;
		}
		else
		{
			set_response(RESPONSE_INVALID);
		}
	}
}

/**
* Updates the I2C Slave Address
*/
void process_set_address()
{
	char *param_token = strtok(NULL, COMMAND_DELIMITER);
	uint16_t param = str2num(param_token);
	if(param > 2 && param < 120)
	{
		cli();
		eeprom_update_byte(&eeprom_twi_address, param);
		TWI0_set_address(param);
		sei();
		// Clear the buffer, address was changed.
		memset(write_buffer, '\0', TWI_BUFFER_SIZE);
	}
	else
	{
		set_response(RESPONSE_INVALID);
	}
}
