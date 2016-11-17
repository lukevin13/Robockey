#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "localize.h"

#define SERIAL_DEBUG	0
#define CHANNEL			0
#define ADDRESS			0x40
#define PACKET_LENGTH	3

void init();

int main() {
	init();

	while (1) {

	}

	return 1;
}

void init() {
	m_clockdivide(0);

	m_red(ON);
	
	m_bus_init();
	m_usb_init();

	while (!m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH))
	while (!m_usb_isconnected() & SERIAL_DEBUG)
	while (!m_wii_open());

	m_red(OFF);
}