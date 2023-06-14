/*
* Copyright (c) 2023, FibStack
* All rights reserved.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/


#ifndef MOTORS_H_
#define MOTORS_H_

#define MOTOR_DEVICES 4
#define COMMAND_BUFFER_SIZE 10

extern uint8_t head;
extern uint8_t tail;

typedef struct
{
	unsigned long steps;
	uint8_t dir;
	uint16_t speed;
	uint16_t counter;
} RunCommand;

extern RunCommand runCommnadsBufferA[];
extern RunCommand runCommnadsBufferB[];
extern RunCommand runCommnadsBufferC[];
extern RunCommand runCommnadsBufferD[];

extern PORT_t* device_ports[];
extern uint8_t device_step_masks[];
extern uint8_t device_dir_masks[];

extern uint8_t is_switch_activated;
extern uint8_t is_paused;

extern RunCommand moveCommand;
extern uint8_t move_device_id;

extern void clear_command_struct(RunCommand* run_command);
extern void clear_command_buffer();
extern void run_command_on_device(RunCommand* run_command, uint8_t device_id);
extern uint8_t is_buffer_command_available(uint8_t index);

#endif /* MOTORS_H_ */