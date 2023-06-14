/*
* Copyright (c) 2023, FibStack
* All rights reserved.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/


#ifndef UTIL_H_
#define UTIL_H_

#define MAX_STR_NUM_SIZE 10

extern uint8_t num_conversion_error;

extern uint16_t str2num(char *str);
extern unsigned long str2ulong(char *str);
extern char* num2str(unsigned long value);

#endif /* UTIL_H_ */