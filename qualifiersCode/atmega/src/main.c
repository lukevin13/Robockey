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
volatile int state = 0;	// Robot state. 0: idle, 1:playing, 2:comtest
volatile int rf_flag = 0;
volatile int score_red = 0;
volatile int score_blue = 0;

void init();
void print_location(double* position, double* theta);
void print_mWii_data(unsigned int* data);
void m_rf_process_state(char* buffer);
void pwm_setup();
void timer_setup();
void turn_off_wheels();

int main() {
	init();
	char mWii_read;
	double r_pos[2] = {0.0, 0.0};
	double r_theta[1] = {0.0};
	unsigned int mWii_buffer[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	char m_rf_buffer[PACKET_LENGTH];

	while (1) {
		if (rf_flag) {
			rf_flag = 0;
			m_rf_read(m_rf_buffer, PACKET_LENGTH);
			m_rf_process_state(m_rf_buffer);
		}

		// Read from mWii and process data
		mWii_read = m_wii_read(mWii_buffer);
		if (mWii_read) {
			int x[4] = {
				mWii_buffer[0]-512, 
				mWii_buffer[3]-512, 
				mWii_buffer[6]-512, 
				mWii_buffer[9]-512
			};

			// Pin hole camera flips the y axis
			int y[4] = {
				mWii_buffer[1] + -512, 
				mWii_buffer[4] + -512, 
				mWii_buffer[7] + -512, 
				mWii_buffer[10] + -512
			};
			int numStars = countNumStars(x, y);
			if (SERIAL_DEBUG) print_mWii_data(mWii_buffer);
			
			if (localize_me(r_theta, r_pos,x,y,numStars)) {

			} else {

			}
		}

		if (SERIAL_DEBUG) print_location(r_pos, r_theta);

		// State logic
		switch (state) {
			case (0): 
				// Idle
				break;
			case(1):
				// Play
				break;
			case(2):
				// ComTest
				if (TIFR3, TOV3) {
					set(TIFR3, TOV3);
					m_green(TOGGLE);
				}
				break;
			default:
				break;
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

	set(DDRB, 5);
	set(DDRB, 6);
	set(DDRB, 7);
	set(DDRB, 0);
	set(DDRB, 1);
	clear(PORTB, 0);
	clear(PORTB, 1);

	pwm_setup();
	while (!m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH));
	while (!m_usb_isconnected() & SERIAL_DEBUG);
	while (!m_wii_open());

	m_red(OFF);
}

void print_location(double* position, double* theta) {
	m_usb_tx_string("Position: (");
	m_usb_tx_int((int)position[0]);
	m_usb_tx_string(", ");
	m_usb_tx_int((int)position[1]);
	m_usb_tx_string(", ");
	m_usb_tx_int((int)theta[0]);
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

// // mrf interrupt
// ISR(INT2_vect) {
// 	rf_flag = 1;
// }

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
	if (command_check(buffer, COM_COMTEST)) state = 2;
	else if (command_check(buffer, COM_PLAY)) {
		state = 1;
		m_green(ON);
	}
	else if (command_check(buffer, COM_HALFTIME)) {
		state = 0;
		turn_off_wheels();
	}
	else if (command_check(buffer, COM_GAMEOVER)) {
		state = 0;
		turn_off_wheels();
	}
	else if (command_check(buffer, COM_PAUSE)) {
		state = 0;
		turn_off_wheels();
	}

	// Update scores and switch to idle state
	if (buffer[0] == 0xA2 || buffer[0] == 0xA3) {
		score_red = buffer[1];
		score_blue = buffer[2];
	}
}

// Set up pwms
void pwm_setup() {
	// clock source /1
	clear(TCCR1B, CS12);
	clear(TCCR1B, CS11);
	clear(TCCR1B, CS10);

	// mode 15
	set(TCCR1B, WGM13);
	set(TCCR1B, WGM12);
	set(TCCR1A, WGM11);
	set(TCCR1A, WGM10);

	// Channel B mode
	set(TCCR1A, COM1B1);
	clear(TCCR1A, COM1B0);

	// Channel C mode
	set(TCCR1A, COM1C1);
	clear(TCCR1A, COM1C0);

	// Set values
	OCR1A = 1600; // 10kHz
	OCR1B = 0;
	OCR1C = 0;

	// Start timer
	set(TCCR1B, CS10);
}

void turn_off_wheels() {
	OCR1B = 0;
	OCR1C = 0;
}

void timer_setup() {
	// Clock source /1024
	set(TCCR3B, CS32);
	clear(TCCR3B, CS31);
	set(TCCR3B, CS30);

	// mode 4
	set(TCCR3B, WGM32);

	// 2 Hz
	OCR3A = 7813;
}