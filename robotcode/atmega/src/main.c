#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "localize.h"

// Constants
#define DEBUG			0		// Serial debug
#define CHANNEL			1
#define ADDRESS			80 		// Robot1:80, Robot2:81, Robot3:82
#define PACKET_LENGTH	10

#define COM_COMTEST		0xA0
#define COM_PLAY		0xA1
#define COM_PAUSE		0xA4
#define	COM_HALFTIME	0xA6
#define COM_GAMEOVER	0xA7

// Volatiles
volatile int state = 0;
volatile int rf_flag = 0;
volatile int lost_flag = 0;

volatile int mrf_buffer[PACKET_LENGTH];
volatile int mWii_buffer[12];

volatile double robot_pos[2];
volatile double robot_theta[1];
volatile double goal_pos[2];
volatile double goal_theta[1];
volatile double goal_dist;

// Function declarations
void init();
void setupPWM();
void checkRF();
void localize();
void calculateAngleToGoal();
void pollIRSensors();
void calculateAngleToPuck();
void chooseStrategy();
void drive();
void update();
void debug();

// Main
int main() {
	init();

	while (1) {
		checkRF();
		localize();
		calculateAngleToGoal();
		pollIRSensors();
		calculateAngleToPuck();
		chooseStrategy();
		drive();
		update();
		debug();
	}

	return 1;
}

// Functions
void init() {
	m_clockdivide(0);
	m_red(ON);

	sei();
	m_bus_init();
	m_usb_init();
	setupPWM();

	while (!m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH));
	while (!m_usb_isconnected() & SERIAL_DEBUG);
	while (!m_wii_open());

	m_red(OFF);
}

// m_rf Interrupt
ISR(INT2_vect) {
	rf_flag = 1;
}

// Set up pwms
void setupPWM() {
	// Set up motor direction ports
	set(DDRB, 0);
	set(DDRB, 1);
	clear(PORTB, 0);
	clear(PORTB, 1);

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
	OCR1A = 1600; // 10kHz
	OCR1B = 0;
	OCR1C = 0;

	// Start timer
	set(TCCR1B, CS10);
}

// Process rf packets
void checkRF() {
	// Gameplay state checks
	if (rf_flag) {
		rf_flag = 0;
		m_rf_read(mrf_buffer, PACKET_LENGTH);

		if (command_check(mrf_buffer, COM_COMTEST)) state = 2;
		else if (command_check(mrf_buffer, COM_PLAY)) {
			state = 1;
			m_green(ON);
		}
		else if (command_check(mrf_buffer, COM_HALFTIME)) {
			state = 0;
			turn_off_wheels();
		}
		else if (command_check(mrf_buffer, COM_GAMEOVER)) {
			state = 0;
			turn_off_wheels();
		}
		else if (command_check(mrf_buffer, COM_PAUSE)) {
			state = 0;
			turn_off_wheels();
		}

		// Update scores
		else if (mrf_buffer[0] == 0xA2 || mrf_buffer[0] == 0xA3) {
			scoreRed = mrf_buffer[1];
			scoreBlue = mrf_buffer[2];
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
		lost_flag = !localize_me(robot_theta,robot_pos,x,y,n);
	}
}

void calculateAngleToGoal() {
	double vector[2] = {goal_pos[0]-robot_pos[0],goal_pos[1]-robot_pos[1]};
	goal_dist = vector[0]*vector[0] + vector[1]*vector[1];
	goal_theta[0] = atan2(1,0) - atan2(vector[1],vector[0]);
}

void drive() {
	double_theta = (-goal_theta[0]-robot_theta[0]);
	if (goal_theta[0] >= 180) goal_theta[0] -= 360;
	if (goal_theta[0] <= -180) goal_theta[0] += 360;

	int l_value = 0;
	int r_value = 0;
	if (abs(goal_theta[0]) < 5) {
		l_value += 85;
		r_value += 85;
		if (goal_theta[0] < 0) {
			l_value += 10;
			r_value -= 10;
		} else {
			l_value -= 10;
			r_value += 10;
		}
	} else {
		if (goal_theta[0] < -5) {
			l_value += 70;
			r_value -= 70;
		} else if (goal_theta[0] > 5) {
			l_value -= 70;
			r_value += 70;
		}
	}
	
	left_drive(l_value);
	right_drive(r_value);
}