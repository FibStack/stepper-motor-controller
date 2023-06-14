/*
* Copyright (c) 2023, FibStack
* All rights reserved.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include <avr/io.h>
#include "tca.h"

uint16_t period = 0x03E8; // Timer period 0x3E8 = 1000

/**
* Initializes the TCA0 peripheral
*/
void TCA0_init()
{
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // Enable interrupt on overflow
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // Disable wave form generation	TCA0.SINGLE.PER = period; // Set the period	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm; // Set the pre-scaler and enable the timer
}