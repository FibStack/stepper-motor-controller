/*
* Copyright (c) 2021, FibStack
* All rights reserved.
* 
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree. 
*/
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "i2clib.h"

int get_slave_access(uint8_t address, bool verbose)
{
    int file_id = open("/dev/i2c-1", O_RDWR);

    if (verbose)
    {
        printf("get_slave_access()\n");
    }

    if (file_id < 0)
    {
        if (verbose)
        {
            printf("Error opening file: %s\n", strerror(errno));
        }
        return -1;
    }

    if (ioctl(file_id, I2C_SLAVE, address) < 0)
    {
        if (verbose)
        {
            printf("Failed to acquire buss access: %s\n", strerror(errno));
        }
        return -1;
    }

    return file_id;
}

char *send_get_data(uint8_t address, char *message, bool verbose)
{
    char read_buffer[MAX_BUFFER_SIZE];
    char *result = malloc(sizeof(char) * MAX_BUFFER_SIZE);
    int file_id = get_slave_access(address, verbose);
    int length = strlen(message) + 1;
    bool is_lib_error = true;

    if (verbose)
    {
        printf("send_get_data()\n");
        printf("Address: %d\n", address);
        printf("Message: %s\n", message);
    }

    if (file_id >= 0)
    {
        if (write(file_id, message, length) != length)
        {
            if (verbose)
            {
                printf("Failed to write to the i2c bus: %s\n", strerror(errno));
            }
        }
        else
        {
            if (verbose)
            {
                printf("Message sent\n");
            }

            int bytes_read = read(file_id, read_buffer, MAX_BUFFER_SIZE);
            if (bytes_read > 0)
            {
                is_lib_error = false;
                strcpy(result, read_buffer);
                close(file_id);
                if (verbose)
                {
                    printf("Message read: %s\n", result);
                }
            }
            else if (verbose)
            {

                printf("Failed to read bytes: %s\n", strerror(errno));
            }
        }
    }

    if (is_lib_error)
    {
        strcpy(result, LIB_ERROR_MSG);
    }

    return result;
}
