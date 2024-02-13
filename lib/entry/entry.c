// 
// Entry point for user code
// 'setup' is called once after device reset
// 'loop' is called repeatedly
//
// Copyright (c) 2024 Jonathan Tainer
//

#include <stdio.h>
#include "main.h"

void setup(void) {

}

void loop(void) {
	printf("Hello world\r\n");
	HAL_Delay(1000);
}
