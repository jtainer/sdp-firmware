// 
// Serial flash loader
// Communicates with a host over UART to program the external flash
//
// Copyright (c) 2024, Jonathan Tainer
//

#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include <stdbool.h>

void loader_listen(bool enable);

uint8_t* loader_data();

void loader_program_flash();

#endif
