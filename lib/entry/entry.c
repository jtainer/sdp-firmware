// 
// Entry point for user code
// 'setup' is called once after device reset
// 'loop' is called repeatedly
//
// Copyright (c) 2024 Jonathan Tainer
//

#include <main.h>
#include <stdio.h>
#include <i2c-mux.h>
#include <Library/w25q_mem.h>
#include <bno055_stm32.h>

extern I2C_HandleTypeDef hi2c1;
const int IMU_COUNT = 8;

i2c_mux_t mux;


void setup(void) {

	printf("Begin I2C multiplexer initialization.....\r\n");
	// Set mux 0 address offset to 0
	HAL_GPIO_WritePin(MUX0_A0_GPIO_Port, MUX0_A0_Pin, 0);
	HAL_GPIO_WritePin(MUX0_A1_GPIO_Port, MUX0_A1_Pin, 0);
	HAL_GPIO_WritePin(MUX0_A2_GPIO_Port, MUX0_A2_Pin, 0);
	// Set mux 1 address offset to 7
	HAL_GPIO_WritePin(MUX1_A0_GPIO_Port, MUX1_A0_Pin, 1);
	HAL_GPIO_WritePin(MUX1_A1_GPIO_Port, MUX1_A1_Pin, 1);
	HAL_GPIO_WritePin(MUX1_A2_GPIO_Port, MUX1_A2_Pin, 1);
	// Setup multiplexer
	mux.hi2c = &hi2c1;
	mux.rst_port = MUX0_RST_GPIO_Port;
	mux.rst_pin = MUX0_RST_Pin;
	mux.addr_offset = 0;
	i2c_mux_reset(&mux);
	printf("End I2C multiplexer initialization.\r\n\n");
	
	printf("Begin IMU initialization...\r\n");
	bno055_assignI2C(&hi2c1);
	for (int i = 0; i < IMU_COUNT; i++) {
		printf("Checking IMU[%d].....\r\n", i);
		i2c_mux_select(&mux, i);
		bno055_setup();
		bno055_setOperationModeNDOF();
	}
	printf("End IMU initialization.\r\n\n");
}

void loop(void) {
	for (int i = 0; i < IMU_COUNT; i++) {
		printf("Begin selecting multiplexer channel %d.....\r\n", i);
		i2c_mux_select(&mux, i);
		printf("End selecting multiplexer channel.\r\n\n", i);

		printf("Begin reading IMU.....\r\n");
		bno055_vector_t v = bno055_getVectorQuaternion();
		printf("End reading IMU.\r\n\n");
		printf("\tIMU[%d]: w=%f,\tx=%f,\ty=%f,\tz=%f\r\n", i, v.w, v.x, v.y, v.z);
	}

	HAL_Delay(1000);
}
