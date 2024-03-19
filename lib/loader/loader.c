// 
// Serial flash loader
// Communicates with a host over UART to program the external flash
// 
// Copyright (c) 2024, Jonathan Tainer
// 

//
// Messages sent from the host to the device consist of a 1-byte opcode
// followed by some number of option bytes. The message is terminated by a line
// feed character ('\n'). The size of the message including the opcode and the
// line feed must not exceed RX_BUFFER_SIZE.
//
// After the device receives a valid message, it will reply to the host with
// LD_ACK. If the received message is invalid, the device will reply with
// LD_NACK. How the host handles each situation is not strictly defined.
//
// The flash loader allocates a buffer in RAM for the host to write to before
// dumping the data out to flash. The size of this buffer is LD_BUFFER_SIZE.
// 
// Message formats:
// "s%d\n" Set working address for loader buffer
// "w%02X...\n" Writes a series of bytes to the loader buffer
// "S%d\n" Set working address for flash
// "X%d\n" Erase flash sector
// "M%d\n" Copy bytes from loader buffer into flash
// "Z" Done programming

#include <main.h>
#include <stdio.h>
#include <string.h>
#include <Library/w25q_mem.h>
#include "loader.h"

#define LD_ACK "loader_rx_cplt\n"
#define LD_NACK "loader_rx_fail\n"

// Buffer incoming messages until a full line is received
#define RX_BUFFER_SIZE 4096
static uint8_t rx_buf[RX_BUFFER_SIZE] = { 0 };
static int rx_idx = 0;
static bool rx_full = false;
extern UART_HandleTypeDef huart1;

// Buffer data before writing to flash
#define LD_BUFFER_SIZE 1024
uint8_t ld_buf[LD_BUFFER_SIZE] = { 0 };
// Loader buffer working address
static int ld_idx = 0;
static volatile bool ld_cplt = false;

// Flash working address
static int fl_idx = 0;

void loader_listen(bool enable) {
	ld_idx = 0;
	rx_idx = 0;
	rx_full = false;
	HAL_UART_Receive_IT(&huart1, rx_buf, 1);
}

uint8_t* loader_data() {
	return ld_buf;
}

void loader_program_flash() {
	ld_cplt = false;
	loader_listen(true);
	while (!ld_cplt);
}

void decode_hex(uint8_t* dst, uint8_t* src, int num_bytes) {
	for (int i = 0; i < num_bytes; i++) {
		uint8_t hi = src[(i*2)+0];
		uint8_t lo = src[(i*2)+1];
		if (hi >= '0' && hi <= '9')
			hi = hi - '0';
		else if (hi >= 'A' && hi <= 'F')
			hi = 10 + hi - 'A';
		if (lo >= '0' && lo <= '9')
			lo = lo - '0';
		else if (lo >= 'A' && lo <= 'F')
			lo = 10 + lo - 'A';
		dst[i] = (hi << 4) | lo;
	}
}

void process_packet() {
	int msg_len = strlen((char*)rx_buf);
	if (msg_len < 1) return;
	// Set loader buffer working address
	else if (rx_buf[0] == 's') {
		int idx = 0;
		if (sscanf((char*)(rx_buf+1), "%d", &idx) == 1) {
			ld_idx = idx;
		}
	}
	// Write to loader buffer
	else if (rx_buf[0] == 'w') {
		int num_bytes = (msg_len - 1) / 2;
		decode_hex(ld_buf+ld_idx, rx_buf+1, num_bytes);
	}
	// Set flash working address
	else if (rx_buf[0] == 'S') {
		int idx = 0;
		if (sscanf((char*)(rx_buf+1), "%d", &idx) == 1) {
			fl_idx = idx;
		}
	}
	// Erase flash sector
	else if (rx_buf[0] == 'X') {
		int addr = 0;
		if (sscanf((char*)(rx_buf+1), "%d", &addr) == 1) {
			W25Q_EraseSector(addr);
		}
	}
	// Copy loader buffer into flash
	else if (rx_buf[0] == 'M') {
		int len = 0;
		if (sscanf((char*)(rx_buf+1), "%d", &len) == 1) {
			W25Q_ProgramRaw(ld_buf, len, fl_idx);
		}
	}
	// Done programming
	else if (rx_buf[0] == 'Z') {
		ld_cplt = true;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
	// If the buffer is overrun before a full line is received, then ignore all received bytes until the next line
	char c = rx_buf[rx_idx];
	if (c == '\n' || c == '\r') { // Reached end of line
		if (!rx_full) {
			// Replace line feed with null terminator
			rx_buf[rx_idx] = 0;
			
			process_packet();

			// Tell host to proceed
			HAL_UART_Transmit(&huart1, (uint8_t*)LD_ACK, strlen(LD_ACK)+1, 100);
		}
		else {
			// Tell host to abort or retransmit last line
			HAL_UART_Transmit(&huart1, (uint8_t*)LD_NACK, strlen(LD_NACK)+1, 100);
		}

		// Reset and wait for next line
		rx_idx = 0;
		rx_full = false;
	}
	else { // Still receiving line
		if (!rx_full) {
			// Continue to read line
			rx_idx++;
			if (rx_idx >= RX_BUFFER_SIZE) {
				// Buffer filled before full line received
				rx_full = true;
				
				// Dump remaining bytes until newline
				rx_idx = 0;
			}
		}
		else {
			// Dump remaining bytes until newline
			rx_idx = 0;
		}
	}

	// Call this function after every byte is received
	if (!ld_cplt) {
		HAL_UART_Receive_IT(&huart1, &rx_buf[rx_idx], 1);
	}
}

