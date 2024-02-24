// 
// Entry point for user code
// 'setup' is called once after device reset
// 'loop' is called repeatedly
//
// Copyright (c) 2024 Jonathan Tainer
//

#include <main.h>
#include <stdio.h>
#include <stdbool.h>
#include <i2c-mux.h>
#include <Library/w25q_mem.h>
#include <bno055_stm32.h>

extern I2C_HandleTypeDef hi2c1;
#define IMU_COUNT 16

i2c_mux_t mux[2] = { 0 };
bool imu_detected[IMU_COUNT] = { 0 };

int select_imu(int channel) {
	int mux_idx = (channel & 0b1000) >> 3;
	int imu_idx = channel & 0b0111;
	int err = i2c_mux_select(&mux[mux_idx], imu_idx);
	// Disable all channels on unused multiplexer
	err |= i2c_mux_select_multi(&mux[!mux_idx], 0);
	return err;
}

void setup(void) {
	// Initialize multiplexers
	mux[0].hi2c = &hi2c1;
	mux[0].rst_port = MUX0_RST_GPIO_Port;
	mux[0].rst_pin = MUX0_RST_Pin;
	mux[0].addr_offset = 0;
	HAL_GPIO_WritePin(MUX0_A0_GPIO_Port, MUX0_A0_Pin, 0);
	HAL_GPIO_WritePin(MUX0_A1_GPIO_Port, MUX0_A1_Pin, 0);
	HAL_GPIO_WritePin(MUX0_A2_GPIO_Port, MUX0_A2_Pin, 0);
	mux[1].hi2c = &hi2c1;
	mux[1].rst_port = MUX1_RST_GPIO_Port;
	mux[1].rst_pin = MUX1_RST_Pin;
	mux[1].addr_offset = 1;
	HAL_GPIO_WritePin(MUX1_A0_GPIO_Port, MUX1_A0_Pin, 1);
	HAL_GPIO_WritePin(MUX1_A1_GPIO_Port, MUX1_A1_Pin, 0);
	HAL_GPIO_WritePin(MUX1_A2_GPIO_Port, MUX1_A2_Pin, 0);
	if (i2c_mux_reset(&mux[0]) != 0) {
		printf("ERROR: Failed to initialize mux[0]\r\n");
		Error_Handler();
	}
	else {
		printf("INFO: Successfully initialized mux[0]\r\n");
	}
	if (i2c_mux_reset(&mux[1]) != 0) {
		printf("ERROR: Failed to initialize mux[1]\r\n");
		Error_Handler();
	}
	else {
		printf("INFO: Successfully initialized mux[1]\r\n");
	}
	
	// Detect which channels have an IMU connected
	for (int i = 0; i < IMU_COUNT; i++) {
		select_imu(i);
		HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, BNO055_I2C_ADDR<<1, 5, 100);
		if (status == HAL_OK) {
			printf("INFO: IMU detected on channel %d\r\n", i);
			imu_detected[i] = true;
		}
		else {
			printf("INFO: No response on channel %d\r\n", i);
			imu_detected[i] = false;
		}
	}
	// Only continue if one or more IMUs are responding
	int active_imu_count = 0;
	for (int i = 0; i < IMU_COUNT; i++) {
		if (imu_detected[i]) active_imu_count++;
	}
	if (active_imu_count == 0) {
		printf("ERROR: No IMUs detected, halting program\r\n");
		Error_Handler();
	}
	else {
		printf("INFO: Proceeding with %d IMUs\r\n", active_imu_count);
	}
	// Initialize IMUs
	bno055_assignI2C(&hi2c1);
	for (int i = 0; i < IMU_COUNT; i++) {
		if (!imu_detected[i]) continue;
		select_imu(i);
		bno055_reset();
		bno055_setup();
		bno055_setOperationModeNDOF();
	}

	// Initialize QSPI flash
	if (W25Q_Init() == W25Q_OK) {
		printf("INFO: Successfully initialized external flash\r\n");
	}
	else {
		printf("ERROR: Failed to initialize external flash\r\n");
		Error_Handler();
	}

	// Test writing/reading
	W25Q_EraseSector(0);
	uint8_t tx_byte = 123;
	uint8_t addr = 0;
	uint32_t page = 0;
	W25Q_EraseSector(0);
	W25Q_ProgramByte(tx_byte, addr, page);
	uint8_t rx_byte = 0;
	W25Q_ReadByte(&rx_byte, addr, page);
	if (tx_byte == rx_byte) {
		printf("INFO: Successfully programmed flash\r\n");
	}
	else {
		printf("ERROR: Failed to program flash\r\n");
		Error_Handler();
	}
}

void loop(void) {
	for (int i = 0; i < IMU_COUNT; i++) {
		if (!imu_detected[i]) continue;
		select_imu(i);
		bno055_vector_t v = bno055_getVectorQuaternion();
		printf("\tIMU[%d]: w=%f,\tx=%f,\ty=%f,\tz=%f\r\n", i, v.w, v.x, v.y, v.z);
	}

	HAL_Delay(1000);
}
