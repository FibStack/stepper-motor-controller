/*
* Copyright (c) 2023, FibStack
* All rights reserved.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/


#include <avr/io.h>
#include "motors.h"

uint8_t head = 0;
uint8_t tail = 0;

// Buffers to store commands for each device
RunCommand runCommnadsBufferA[COMMAND_BUFFER_SIZE];
RunCommand runCommnadsBufferB[COMMAND_BUFFER_SIZE];
RunCommand runCommnadsBufferC[COMMAND_BUFFER_SIZE];
RunCommand runCommnadsBufferD[COMMAND_BUFFER_SIZE];

// Predefined ports and mask for each motor
PORT_t* device_ports[MOTOR_DEVICES] = {&PORTC, &PORTC, &PORTA, &PORTA};
uint8_t device_step_masks[MOTOR_DEVICES] = {PIN0_bm, PIN2_bm, PIN4_bm, PIN6_bm};
uint8_t device_dir_masks[MOTOR_DEVICES] = {PIN1_bm, PIN3_bm, PIN5_bm, PIN7_bm};
	
uint8_t is_switch_activated = 0; // Indicates that a limit switch is activated
uint8_t is_paused = 0; // Indicates if commands are paused or not

// Special move command
RunCommand moveCommand = {0, 0, 0, 0};
uint8_t move_device_id = MOTOR_DEVICES;

/**
* Helper function to clear a command
*/
void clear_command_struct(RunCommand* run_command)
{
	run_command->steps = 0;
	run_command->dir = 0;
	run_command->speed = 0;
	run_command->counter = 0;
}

/**
* Helper function to clear all the commands in the buffer
*/
void clear_command_buffer()
{
	for(int i = 0; i < COMMAND_BUFFER_SIZE; i++) {
		clear_command_struct(&runCommnadsBufferA[i]);
		clear_command_struct(&runCommnadsBufferB[i]);
		clear_command_struct(&runCommnadsBufferC[i]);
		clear_command_struct(&runCommnadsBufferD[i]);
	}
}

/**
* Runs one steps from the command
*/
void run_command_on_device(RunCommand* run_command, uint8_t device_id)
{
	// Skip if no steps to run
	if(run_command->steps == 0) {
		return;
	}
	
	run_command->counter++;
	
	if(run_command->counter >= run_command->speed) {
		
		// Set direction
		if(run_command->dir)
		{
			device_ports[device_id]->OUT |= device_dir_masks[device_id];
		}
		else
		{
			device_ports[device_id]->OUT &= ~device_dir_masks[device_id];
		}
		// Toggle step pin
		device_ports[device_id]->OUT ^= device_step_masks[device_id]; // toggle step pin
		if((device_ports[device_id]->OUT & device_step_masks[device_id]) == 0) {
			run_command->steps--;
		}
		run_command->counter = 0;
	}
}

/**
* Helper function to check if there is a command with steps to be executed.
*/
uint8_t is_buffer_command_available(uint8_t index)
{
	if (index >= COMMAND_BUFFER_SIZE)
	{
		return 0;
	}
	
	return runCommnadsBufferA[index].steps > 0 || runCommnadsBufferB[index].steps > 0 ||
		runCommnadsBufferC[index].steps > 0 || runCommnadsBufferD[index].steps > 0;
}