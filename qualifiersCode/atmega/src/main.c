#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "localize.h"

#define SERIAL_DEBUG	1
#define CHANNEL			0
#define ADDRESS			0x40
#define PACKET_LENGTH	3

void init();
void print_location(double* position);
void print_mWii_data(unsigned int* data);

int main() {
	init();
	char mWii_read;
	double r_pos[2] = {0.0, 0.0};
	unsigned int mWii_buffer[12];

	while (1) {

		// Read from mWii and process data
		mWii_read = m_wii_read(mWii_buffer);
		if (mWii_read) {
			unsigned int x[4] = {mWii_buffer[0], mWii_buffer[3], mWii_buffer[6], mWii_buffer[9]};
			unsigned int y[4] = {mWii_buffer[1], mWii_buffer[4], mWii_buffer[7], mWii_buffer[10]};
			if (localize_me(r_pos, x, y)) {

			} else {

			}

			if (SERIAL_DEBUG) {
				print_mWii_data(mWii_buffer);
				print_location(r_pos);
			}
		}
	}

	return 1;
}

void init() {
	m_clockdivide(0);

	m_red(ON);
	
	m_bus_init();
	m_usb_init();

	// while (!m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH))
	while (!m_usb_isconnected() & SERIAL_DEBUG)
	while (!m_wii_open());

	m_red(OFF);
}

void print_location(double* position) {
	m_usb_tx_string("Position: (");
	m_usb_tx_int((int)position[0]);
	m_usb_tx_string(", ");
	m_usb_tx_int((int)position[1]);
	m_usb_tx_string(")\r\n");
}

void print_mWii_data(unsigned int* data) {
	m_usb_tx_string("mWii data: ");
	int i;
	for (i=0;i<11;i++) {
		m_usb_tx_uint(data[i]);
		m_usb_tx_string(", ");
	}
	m_usb_tx_uint(data[11]);
	m_usb_tx_string("\r\n");
}