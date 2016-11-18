#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "localize.h"

// Constants
#define SERIAL_DEBUG	1
#define CHANNEL			1
#define ADDRESS			80 		// Robot1:80, Robot2:81, Robot3:82
#define PACKET_LENGTH	10

#define COM_COMTEST		0xA0
#define COM_PLAY		0xA1
#define COM_PAUSE		0xA4
#define	COM_HALFTIME	0xA6
#define COM_GAMEOVER	0xA7

// Volatiles
volatile int state = 0;	// Robot state. 0: idle, 1:playing
volatile int rf_flag = 0;

void init();
void print_location(double* position);
void print_mWii_data(unsigned int* data);
void m_rf_process_state(char* buffer);

int main() {
	init();
	char mWii_read;
	double r_pos[2] = {0.0, 0.0};
	unsigned int mWii_buffer[12];
	char m_rf_buffer[PACKET_LENGTH];

	while (1) {
		if (rf_flag) {
			m_rf_read(m_rf_buffer, PACKET_LENGTH);
			m_rf_process_state(m_rf_buffer);
		}

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
	sei();
	m_bus_init();
	m_usb_init();

	while (!m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH))
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

// mrf interrupt
ISR(INT2_vect) {
	rf_flag = 1;
}

int command_check(char* buffer, char command) {
	int i;
	for (i=0;i<PACKET_LENGTH;i++) {
		if (buffer[i] != command) return 0;
	}
	return 1;
}

// Process mrf packets and change robot state
void m_rf_process_state(char* buffer) {
	// Gameplay state checks
	if (command_check(buffer, COM_COMTEST)) state = 0;
	if (command_check(buffer, COM_PLAY)) state = 1;
	if (command_check(buffer, COM_HALFTIME)) state = 0;
	if (command_check(buffer, COM_GAMEOVER)) state = 0;
	if (command_check(buffer, COM_PAUSE)) state = 0;

	// Update scores and switch to idle state
	if (buffer[0] == 0xA2) {

	}
	if (buffer[0] == 0xA3) {

	}
}
