#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "localize.h"
#include <math.h>

// Constants
#define SERIAL_DEBUG	1		// Serial debug
#define DEBUG_ROBOT		1
#define DEBUG_TARGET	0
#define DEBUG_MWII		0
#define DEBUG_MRF		0
#define DEBUG_ADC		0

#define CHANNEL			1
#define ADDRESS			80 		// Robot1:80, Robot2:81, Robot3:82
#define PACKET_LENGTH	10
#define NUM_ADC			8
#define RAD2DEG 		180/3.14
#define FORTYFIVEDEG	0.7854
// #define FORTYFIVEDEG	0.2

#define COM_COMTEST		((char) 0xA0)
#define COM_PLAY		((char) 0xA1)
#define COM_PAUSE		((char) 0xA4)
#define COM_DETANGLE	((char) 0xA5)
#define	COM_HALFTIME	((char) 0xA6)
#define COM_GAMEOVER	((char) 0xA7)


// Volatiles
volatile int redTeam = 1;		// 0: Blue Team, 1: Red Team
volatile int scoreRed = 0;
volatile int scoreBlue = 0;

volatile int state = 1;
volatile int rf_flag = 0;
volatile int rf_debug_flag = 0;
volatile int lost_flag = 0;
volatile int led_on_flag = 0;
volatile int puck_unseen = 0;
volatile int have_puck = 0;

char mrf_buffer[PACKET_LENGTH];
unsigned int mWii_buffer[12];

uint16_t adc_val[NUM_ADC];
double robot_pos[2];
double robot_theta[1];
double goal_pos[2];
double goal_theta[1];
double target_pos[2];
double target_theta[1];
double goal_dist;
double target_dist;
double puck_theta;
int pt = -1;
double err;

// Function Declarations
void init();
void setupGPIO();
void setupPWM();
void setupTimer();
void setupADC();
void checkRF();
void localize();
void calculateAngleToGoal();
void findPuck();
void adc0();
void adc1();
void adc4();
void adc5();
void adc6();
void adc7();
void adc8();
void adc9();
void chooseStrategy();
void face();
void forward();
void drive();
void left_drive();
void right_drive();
void turn_off_wheels();
void update();
void debug();
void ledOn();
void ledOff();
void ledToggle();

// Main
int main() {
	init();

	if (redTeam) {
		goal_pos[0] = 20;
		goal_pos[1] = -120.0;
	} 
	else {
		goal_pos[0] = 20;
		goal_pos[0] = 120.0;
	}
	m_wait(1500);

	while (1) {
		checkRF();

		switch (state) {
			case 0: {
				if (check(TIFR3, OCF3A)) {
					set(TIFR3, OCF3A);
					ledToggle();
				}
				break;
			}
			case 1: {
				localize();
				calculateAngleToGoal();
				findPuck();
				// chooseStrategy();
				update();
				break;
			}
			default: {

			}
		}

		debug();
	}

	return 1;
}

// Initialization Function
void init() {
	m_clockdivide(0);
	m_red(ON);

	sei();
	m_bus_init();
	m_usb_init();
	setupGPIO();
	setupPWM();
	setupTimer();
	setupADC();

	while (!m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH));
	while (!m_usb_isconnected() & SERIAL_DEBUG);
	while (!m_wii_open());

	m_red(OFF);
}

// m_rf Interrupt
ISR(INT2_vect) {
	rf_flag = 1;
}

// Set up IO Pins
void setupGPIO() {
	// Set up motor direction ports
	set(DDRB, 0);
	set(DDRB, 1);
	clear(PORTB, 0);
	clear(PORTB, 1);

	// Set up LED ports
	set(DDRB, 2);		// Red LED
	set(DDRB, 3);		// Blue LED
	clear(PORTB, 2);
	clear(PORTB, 3);
}

// Set up pwms
void setupPWM() {
	// Set up pwm
	set(DDRB, 5);
	set(DDRB, 6);
	set(DDRB, 7);

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
	OCR1A = 16000; // 1kHz
	OCR1B = 0;
	OCR1C = 0;

	// Start pwm
	set(TCCR1B, CS10);
}

// Set up timers
void setupTimer() {
	// Clock source /1024
	set(TCCR3B, CS32);
	clear(TCCR3B, CS31);
	set(TCCR3B, CS30);

	// mode 4
	set(TCCR3B, WGM32);

	// 2 Hz
	OCR3A = 7813;
}

// Set up ADC
void setupADC() {
	// Access F4-F7
	m_disableJTAG();

	// Set voltage reference Vcc
	clear(ADMUX, REFS1);
	set(ADMUX, REFS0);

	// Set ADC prescaler /64
	set(ADCSRA, ADPS2);
	set(ADCSRA, ADPS1);
	clear(ADCSRA, ADPS0);

	// Disable digital inputs
	set(DIDR0, ADC0D);
	set(DIDR0, ADC1D);
	set(DIDR0, ADC4D);

	// Enable free running mode
	set(ADCSRA, ADATE);
}

// Checks packets for game commands
int command_check(char* buffer, char command) {
	int i;
	for (i=0;i<PACKET_LENGTH;i++) {
		if (buffer[i] != command) return 0;
	}
	return 1;
}

// Process rf packets
void checkRF() {
	// Gameplay state checks
	if (rf_flag) {
		rf_flag = 0;
		rf_debug_flag = 1;
		m_rf_read(mrf_buffer, PACKET_LENGTH);

		int var = (int) mrf_buffer[0];
		if (var == COM_COMTEST) {
			turn_off_wheels();
			state = 0;
		}
		else if (var == COM_PLAY) {
			state = 1;
			ledOff();
			ledOn();
		}
		else if (var == COM_HALFTIME) {
			state = 6;
			turn_off_wheels();
		}
		else if (var == COM_GAMEOVER) {
			state = 4;
			turn_off_wheels();
		}
		else if (var == COM_PAUSE) {
			state = 4;
			turn_off_wheels();
		}
		else if (var == COM_DETANGLE) {
			state = 5;
			turn_off_wheels();
		}

		if (mrf_buffer[0] == 0xA2 || mrf_buffer[0] == 0xA3) {
			scoreRed = mrf_buffer[1];
			scoreBlue = mrf_buffer[2];
			turn_off_wheels();
		}

		// Custom commands
		else {

		}
	}
}

// Determine location
void localize() {
	if (m_wii_read(mWii_buffer)) {
		int x[4] = {
			mWii_buffer[0]-512, 
			mWii_buffer[3]-512, 
			mWii_buffer[6]-512, 
			mWii_buffer[9]-512
		};

		// Pin hole camera flips the y axis
		int y[4] = {
			-mWii_buffer[1]+384, 
			-mWii_buffer[4]+384, 
			-mWii_buffer[7]+384, 
			-mWii_buffer[10]+384
		};

		int n = countNumStars(x,y);
		if (n==4) {
			localize_me(robot_theta,robot_pos,x,y,n);
			lost_flag = 0;
		}
		else lost_flag = 1;
	}
}

// Calculate angle to goal
void calculateAngleToGoal() {
	double vector[2] = {goal_pos[0]-robot_pos[0],goal_pos[1]-robot_pos[1]};
	goal_dist = sqrt(vector[0]*vector[0] + vector[1]*vector[1]);
	goal_theta[0] = atan2(1,0) - atan2(vector[1],vector[0]);
}


// Find the puck
void findPuck() {
	// Read ADC
	adc0();
	adc1();
	adc4();
	adc5();
	adc6();
	adc7();
	adc8();
	adc9();

	// Determine max
	int max_index = -1;
	int max_val = 0;
	int i;
	for (i=0;i<3;i++) {
		if (adc_val[i] > max_val && adc_val[i] > 10) {
			max_index = i;
			pt = i;
			max_val = adc_val[i];
		}
	}
	if (max_index == -1) puck_unseen = 1;
	else puck_unseen = 0;
	if (max_index == 1 && max_val > 980) have_puck = 1;
	// else have_puck = 0;
}

// Choose Strategy
void chooseStrategy() {
	have_puck = 1;
	if (!have_puck) {
		if (!puck_unseen) {
			if (pt == 0) err = adc_val[1] - adc_val[0];
			if (pt == 1) err = 0;
			if (pt == 2) err = adc_val[1] - adc_val[2];
			err *= 0.2;
			right_drive(80 + err);
			left_drive(80 + err);
		} else {
			if (pt <= 0) {
				right_drive(-75);
				left_drive(75);
			} else if (pt == 2) {
				right_drive(75);
				left_drive(-75);
			}
		}
	} else {
		target_theta[0] = goal_theta[0];
		target_pos[0] =  goal_pos[0];
		target_pos[1] = goal_pos[1];
		if (!lost_flag) drive();
		// turn_off_wheels();
	}
}

// Drive motors to target location
void drive() {
	double theta = (-target_theta[0]-robot_theta[0])*RAD2DEG;
	if (theta >= 180) theta -= 360;
	if (theta <= -180) theta += 360;
	theta = theta * 0.4;

	if (target_dist > 1) {
		right_drive(20+theta);
		left_drive(20-theta);
	}
}

// Face a direction
void face() {
	double theta = (-target_theta[0]-robot_theta[0])*RAD2DEG;
	if (theta >= 180) theta -= 360;
	if (theta <= -180) theta += 360;

	if (theta < -15) {
		left_drive(90);
		right_drive(-90);
	} else if (theta > 15) {
		left_drive(-90);
		right_drive(90);
	}
}

// Forward
void forward() {
	left_drive(40);
	right_drive(40);
}

// Left motor. direction B0, pwm OCR1B
void right_drive(int value) {
	if (value > 0) clear(PORTB, 0);
	else set(PORTB, 0);
	if (abs(value) < 2) OCR1B = 0;
	else {
		double dt = 16000.0*(abs(value)/100.0);
		OCR1B = (int)dt;
	}
}

// Right motor. direction B1, pwm OCR1C
void left_drive(int value) {
	if (value < 0) clear(PORTB, 1);
	else set(PORTB, 1);
	if (abs(value) < 2) OCR1C = 0;
	else {
		double dt = 16000.0*(abs(value)/100.0);
		OCR1C = (int)dt;
	}
}

// Stops both wheels
void turn_off_wheels() {
	OCR1B = 0;
	OCR1C = 0;
}

// Update variables
void update() {
	target_dist = pdist(target_pos, robot_pos);
}

// Debug
void debug() {
	if (!SERIAL_DEBUG) return;

	if (DEBUG_ROBOT) {
		// Print Robot (x,y,theta)
		m_usb_tx_string("Robot: (");
		m_usb_tx_int((int)robot_pos[0]);
		m_usb_tx_string(", ");
		m_usb_tx_int((int)robot_pos[1]);
		m_usb_tx_string(", ");
		m_usb_tx_int((int)(robot_theta[0]*RAD2DEG));
		m_usb_tx_string(")\n");
	}

	if (DEBUG_TARGET) {
		// Print Target (x,y,theta)
		m_usb_tx_string("Robot: (");
		m_usb_tx_int((int)target_pos[0]);
		m_usb_tx_string(", ");
		m_usb_tx_int((int)target_pos[1]);
		m_usb_tx_string(", ");
		m_usb_tx_int((int)target_theta[0]);
		m_usb_tx_string(")\n");
	}

	if (DEBUG_MWII) {
		// Print mWii buffer
		m_usb_tx_string("mWii: [");
		int i;
		for (i=0;i<11;i++) {
			m_usb_tx_uint(mWii_buffer[i]);
			m_usb_tx_string(", ");
		}
		m_usb_tx_uint(mWii_buffer[11]);
		m_usb_tx_string("]\n");
	}

	if (DEBUG_MRF && rf_debug_flag) {
		// Print mWii buffer
		rf_debug_flag = 0;
		m_usb_tx_string("mRF: [");
		int i;
		for (i=0;i<PACKET_LENGTH-1;i++) {
			m_usb_tx_hexchar(mrf_buffer[i]);
			m_usb_tx_string(", ");
		}
		m_usb_tx_hexchar(mrf_buffer[PACKET_LENGTH-1]);
		m_usb_tx_string("]\t");

		// Print Robot state
		m_usb_tx_string("State: ");
		m_usb_tx_int(state);
		m_usb_tx_string("\n");
	}

	if (DEBUG_ADC) {
		// Print ADC values
		m_usb_tx_string("ADC: [");
		int i;
		for (i=0;i<NUM_ADC-1;i++) {
			m_usb_tx_uint(adc_val[i]);
			m_usb_tx_string(", ");
		}
		m_usb_tx_uint(adc_val[NUM_ADC-1]);
		m_usb_tx_string("] Puck Theta: ");
		m_usb_tx_int((int) puck_theta*RAD2DEG);
		m_usb_tx_string(" Have puck: ");
		m_usb_tx_int(have_puck);
		m_usb_tx_string("\n");
	}

}

void ledOn() {
	if (redTeam) set(PORTB, 2);
	else set(PORTB, 3);
	led_on_flag = 1;
}

void ledOff() {
	clear(PORTB, 2);
	clear(PORTB, 3);
	led_on_flag = 0;
}

void ledToggle() {
	if (led_on_flag) ledOff();
	else ledOn();
}


// Read ADC ports
void adc0() {
	// Disable ADC
	clear(ADCSRA, ADEN);

	// F0
	clear(ADCSRB, MUX5);
	clear(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	clear(ADMUX, MUX0);

	// Enable ADC and start conversion
	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	// Wait for conversion to complete and clear flag
	while(!check(ADCSRA, ADIF));
	adc_val[0] = ADC;
	set(ADCSRA, ADIF);
}

void adc1() {
	// Disable ADC
	clear(ADCSRA, ADEN);

	// F1
	clear(ADCSRB, MUX5);
	clear(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	set(ADMUX, MUX0);

	// Enable ADC and start conversion
	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	// Wait for conversion to complete and clear flag
	while(!check(ADCSRA, ADIF));
	adc_val[1] = ADC;
	set(ADCSRA, ADIF);
}

void adc4() {
	// Disable ADC
	clear(ADCSRA, ADEN);

	// F4
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	clear(ADMUX, MUX0);

	// Enable ADC and start conversion
	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	// Wait for conversion to complete and clear flag
	while(!check(ADCSRA, ADIF));
	adc_val[2] = ADC;
	set(ADCSRA, ADIF);
}

void adc5() {
	// Disable ADC
	clear(ADCSRA, ADEN);

	// F5
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	set(ADMUX, MUX0);

	// Enable ADC and start conversion
	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	// Wait for conversion to complete and clear flag
	while(!check(ADCSRA, ADIF));
	adc_val[3] = ADC;
	set(ADCSRA, ADIF);
}

void adc6() {
	// Disable ADC
	clear(ADCSRA, ADEN);

	// F6
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	set(ADMUX, MUX1);
	clear(ADMUX, MUX0);

	// Enable ADC and start conversion
	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	// Wait for conversion to complete and clear flag
	while(!check(ADCSRA, ADIF));
	adc_val[4] = ADC;
	set(ADCSRA, ADIF);
}

void adc7() {
	// Disable ADC
	clear(ADCSRA, ADEN);

	// F7
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	set(ADMUX, MUX1);
	set(ADMUX, MUX0);

	// Enable ADC and start conversion
	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	// Wait for conversion to complete and clear flag
	while(!check(ADCSRA, ADIF));
	adc_val[5] = ADC;
	set(ADCSRA, ADIF);
}

void adc8() {
	// Disable ADC
	clear(ADCSRA, ADEN);

	// D4
	set(ADCSRB, MUX5);
	clear(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	clear(ADMUX, MUX0);

	// Enable ADC and start conversion
	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	// Wait for conversion to complete and clear flag
	while(!check(ADCSRA, ADIF));
	adc_val[6] = ADC;
	set(ADCSRA, ADIF);
}

void adc9() {
	// Disable ADC
	clear(ADCSRA, ADEN);

	// D6
	set(ADCSRB, MUX5);
	clear(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	set(ADMUX, MUX0);

	// Enable ADC and start conversion
	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	// Wait for conversion to complete and clear flag
	while(!check(ADCSRA, ADIF));
	adc_val[7] = ADC;
	set(ADCSRA, ADIF);
}