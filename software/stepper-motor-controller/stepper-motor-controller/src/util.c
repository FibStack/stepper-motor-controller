/*
* Copyright (c) 2023, FibStack
* All rights reserved.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

uint8_t num_conversion_error = 0;

/**
* Converts a string to a 16-bit unsigned integer, 
* sets num_conversion_error variable if an invalid character found
*/
uint16_t str2num(char *str)
{
	uint16_t result = 0;
	num_conversion_error = 0;
	if(str != NULL) {
		short len = strlen(str);
		for (short i = 0; i < len; i++) {
			if(str[i] >= '0' && str[i] <= '9')
			{
				result = result * 10 + (str[i] - '0');
			}
			else
			{
				num_conversion_error = 1; // invalid character detected
				return -1;
			}
		}
	}
	return result;
}

/**
* Converts a string to an unsigned long,
* sets num_conversion_error variable if an invalid character found
*/
unsigned long str2ulong(char *str)
{
	unsigned long result = 0;
	num_conversion_error = 0;
	if(str != NULL) {
		short len = strlen(str);
		for (short i = 0; i < len; i++) {
			if(str[i] >= '0' && str[i] <= '9')
			{
				result = result * 10 + (str[i] - '0');
			}
			else
			{
				num_conversion_error = 1; // invalid character detected
				return -1;
			}
		}
	}
	return result;
}

/**
* Converts an unsigned integer value to a string
*/
char* num2str(unsigned long value)
{
	// the max value of uint16_t has 5 digits, last character is null termination
	char *str_value = malloc(sizeof(char)*MAX_STR_NUM_SIZE);
	memset(str_value, '\0', MAX_STR_NUM_SIZE);
	str_value[0] = '0'; // Initialize with zero to now show empty data
	uint8_t length = 0;
	
	while(value > 0)
	{
		uint8_t digit = value % 10;
		value = value / 10;
		// shift existing characters to the right
		if(length > 0) {
			for(uint8_t i = length; i > 0; i--)
			{
				str_value[i] = str_value[i-1];
			}
		}
		str_value[0] = '0' + digit;
		length++;
	}
	
	return str_value;
}