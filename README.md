# Stepper Motor Controller

## Overview

This repository includes the source files for the Stepper Motor Control board, capable of controlling the direction, steps, and speed of up to four stepper motors concurrently through the I2C interface. 

Kindly note that the project is currently in the prototype phase, and further changes and updates are expected.


## Project organization

- **hardware/** - this folder contains the board circuit schematic and PCB design files.
- **software/util** - contains the source code of a simple CLI application to interact with the board.
- **software/stepper-motor-controller** - contains the firmware source code for ATTiny devices.

## Development tools
- **[Microchip Studio 7.0](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio)**: The IDE for writing and building source code for the ATTiny chip. We also use this tool to flash the chip.
- **[ATMEL-ICE](https://www.microchip.com/en-us/development-tool/ATATMEL-ICE)**: Hardware tool for flashing program and EEPROM memories.
- **GCC** compiler: To complie C utility code on Raspberry Pi.
- **[KiCad EDA](https://www.kicad.org/)**: Schematic and PCB design.

To construct the PCB, please export the gerber files according to the manufacturer's guidelines.

To build the CLI utility, navigate to the `util` folder and execute the `make` command. Please note that this works only on **Raspberry Pi OS**.

To build the ATTiny826 firmware, open the project in Microchip Studio. Build the solution to generate the `*.HEX` and `*.EEP` files. Next, use the appropriate tool available to flash the chip.

## BOM

| # | Components | Recommended models | Footprint | Quantity |
| --- | --- | --- | --- | --- |
| 1 | PCB | Recommended two-layer PCB board of 1.6 mm thickness | | 1 |
| 2 | U1 | SN74LS148 | SOIC-16, 3.9x9.9mm, pitch: 1.27mm | 1 |
| 3 | U2 | ATtiny826 | SOIC-20, 7.5x12.8mm, pitch: 1.27mm | 1 |
| 4 | C1, C4 | Any 1uF 50V SMD capacitor | 0603 (1608 metric) | 2 |
| 5 | C2 | Any 10nF 50V SMD capacitor | 0603 (1608 metric) | 1 |
| 6 | C3, C5 | Any 100nF 50V SMD capacitor | 0603 (1608 metric) | 2 |
| 7 | R1 | Any 470 - 1K 5% 1/4W SMD Resistor | 0603 (1608 metric) | 1 |
| 8 | R2-R13 | Any 10K 5% 1/4W SMD Resistor | 0603 (1608 metric) | 12 |
| 9 | Q1, Q2 | 2N7002 N-channel MOSFET transistor, 60V 300mA | SOT-23-2 | 2 |
| 10 | D1 | SMD LED RED | 0603 (1608 metric) | 1 |
| 11 | J1 | Extended GPIO Male Female Header, 2x20, pitch 2.54mm | 2x20 - P2.54mm | 1 |
| 12 | J2 | Optional, Male Header Connector, 2x3, pitch 2.54mm | 2x3 - P2.54mm | 1 |
| 13 | J3 | Any male connector, 1x2, pitch 2.54mm | 1x2 - P2.54mm | 1 |
| 14 | J4 | Male Header Connector, 2x8, pitch 2.54mm | 2x8 - P2.54mm | 1 |
| 15 | J5, J6, J7, J8 | Male Header Connector, 1x4, pitch 2.54mm | 1x4 - P2.54mm | 4 |
| 16 | SW1 | SPDT 3-pin vertical slide switch | P2.54mm | 1 |


## Documentation

For more detailed information about the board and the I2C protocol, please visit the [project page](https://fibstack.com/projects/stepper-motor-controller/).

## License Information
This product is open source!

Please review the LICENSE.md file for license information. Distributed as-is; no warranty is given.