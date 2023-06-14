/*
* Copyright (c) 2021, FibStack
* All rights reserved.
* 
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree. 
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2clib.h"


int main(int argc, char **argv){
	
	if (argc > 1) {
		// The board I2C address is hardcoded, change it if you use different address
		char* result = send_get_data(0x50, argv[1], false);
		printf("%s\n",result);
	} else {
		printf("Message argument was not provided.\n");
	}
	
	return 1;
}
