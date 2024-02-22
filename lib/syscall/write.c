#include <main.h>
#include <usb_device.h>
#include <usbd_cdc_if.h>
#include <stdio.h>

// Override _write definition so stdout is directed to USB
int _write(int file, char* ptr, int len) {
	// Block until USB port is available for transmitting
	while (CDC_Transmit_FS((uint8_t*) ptr, len) == USBD_BUSY);
	return len;
}
