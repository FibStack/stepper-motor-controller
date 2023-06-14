/*
* Copyright (c) 2021, FibStack
* All rights reserved.
* 
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree. 
*/

#ifndef I2CLIB_H_
#define I2CLIB_H_

#define RESPONSE_INVALID "INVALID"
#define LIB_ERROR_MSG "lib error"
#define I2C_ADDRESS_MIN 0x03
#define I2C_ADDRESS_MAX 0x77
#define MAX_BUFFER_SIZE 150

/**
 * function: get_slave_access()
 * 
 * Returns file ID for the i2c device if open successfully. Returns -1 if an error occured.
 * Every get/send function will close the file.
 * @parameter address - i2c device address
 * @parameter verbose - print additional details
 * 
 */
extern int get_slave_access(uint8_t address, bool verbose);

/**
 * function: send_get_data()
 * 
 * Writes the message to the i2c device address file and reads back the response.
 * Returns the string read or "lib error" if the file couldn't be read
 * This function will open and close the i2c access file.
 * @parameter address - i2c device address
 * @parameter message - the message to be sent to the device
 * @parameter verbose - print additional details
 * 
 */
extern char *send_get_data(uint8_t address, char *message, bool verbose);

#endif /* I2CLIB_H_ */
