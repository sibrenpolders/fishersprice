#include "usb_controller.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include "capser.c"
#include <iostream>
using namespace std;

/* baudrate settings are defined in <asm/termbits.h>, which is
 included by <termios.h> */
#define BAUDRATE B9600            

const char* USB_Controller::DELIM = ":";
const int USB_Controller::SWITCH = 0;
const int USB_Controller::PUSH = 1;
const int USB_Controller::ROTATION = 2;
const int USB_Controller::POTENTIO_METER = 3;
const int USB_Controller::ACCEL = 4;
const int USB_Controller::SOURCE = 5;
const char* USB_Controller::HIGH = "SH";
const char* USB_Controller::LOW = "SL";

USB_Controller::USB_Controller(GUIManager* guiMan) {
	m_guiMan = guiMan;
	accel_values[0] = 0;
	accel_values[1] = 0;
	accel_values[2] = 0;
	m_switch_on = false;
	rotation_value = 0;
	potentiometer_value = 0;
	push = false;
}

bool USB_Controller::init(std::string device_name) {
	fd = open(device_name.c_str(), O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror(device_name.c_str());
		return false;
	}
	tcgetattr(fd, &oldtio); /* save current serial port settings */
	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

	/*
	 BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
	 CRTSCTS : output hardware flow control (only used if the cable has
	 all necessary lines. See sect. 7 of Serial-HOWTO)
	 CS8     : 8n1 (8bit,no parity,1 stopbit)
	 CLOCAL  : local connection, no modem contol
	 CREAD   : enable receiving characters
	 */
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

	/*
	 IGNPAR  : ignore bytes with parity errors
	 ICRNL   : map CR to NL (otherwise a CR input on the other computer
	 will not terminate input)
	 otherwise make device raw (no other input processing)
	 */
	newtio.c_iflag = IGNPAR | ICRNL;
	/*
	 Raw output.
	 */
	newtio.c_oflag = 0;

	/*
	 ICANON  : enable canonical input
	 disable all echo functionality, and don't send signals to calling program
	 */
	newtio.c_lflag = ICANON;

	/*
	 initialize all control characters
	 default values can be found in /usr/include/termios.h, and are given
	 in the comments, but we don't need them here
	 */
	newtio.c_cc[VINTR] = 0; /* Ctrl-c */
	newtio.c_cc[VQUIT] = 0; /* Ctrl-\ */
	newtio.c_cc[VERASE] = 0; /* del */
	newtio.c_cc[VKILL] = 0; /* @ */
	newtio.c_cc[VEOF] = 4; /* Ctrl-d */
	newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
	newtio.c_cc[VMIN] = 1; /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC] = 0; /* '\0' */
	newtio.c_cc[VSTART] = 0; /* Ctrl-q */
	newtio.c_cc[VSTOP] = 0; /* Ctrl-s */
	newtio.c_cc[VSUSP] = 0; /* Ctrl-z */
	newtio.c_cc[VEOL] = 0; /* '\0' */
	newtio.c_cc[VREPRINT] = 0; /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0; /* Ctrl-u */
	newtio.c_cc[VWERASE] = 0; /* Ctrl-w */
	newtio.c_cc[VLNEXT] = 0; /* Ctrl-v */
	newtio.c_cc[VEOL2] = 0; /* '\0' */

	/*
	 now clean the modem line and activate the settings for the port
	 */
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	return true;
}

USB_Controller::~USB_Controller() {
	tcsetattr(fd, TCSANOW, &oldtio);
}

int USB_Controller::update() {
	/* read blocks program execution until a line terminating character is
	 input, even if more than 255 chars are input. If the number
	 of characters read is smaller than the number of chars available,
	 subsequent reads will return the remaining chars. res will be set
	 to the actual number of characters actually read */
	char buf[255];
	int res = read(fd, buf, 255);
	buf[res] = '\0'; /* set end of string, so we can printf */
	if (res > 8) {
		m_guiMan->setText(GUI_ID_RECEIVED_ARDUINO, std::string(buf));
		parse(buf);
	}

	return res;
}

void USB_Controller::parse(char* string) {
	char* res;
	char tmp[512];
	strcpy(tmp, string);
	res = strtok(string, DELIM);

	int substring_number = 0;
	while (res != NULL) {
		setValue(substring_number, res);
		res = strtok(NULL, DELIM);
		++substring_number;
	}

	if (substring_number != 6) {
		cerr << "ERROR: something wrong in serial communication.\n";
	}
}

void USB_Controller::setValue(int substring_number, char* value) {
	switch (substring_number) {
	case ACCEL:
		set_accel(value);
		break;
	case SWITCH:
		set_switch_on(value);
		break;
	case PUSH:
		set_push_on(value);
		break;
	case ROTATION:
		set_rotation_value(value);
		break;
	case POTENTIO_METER:
		set_potentiometer_value(value);
		break;
	case SOURCE:
		;
		break;
	default:
		cerr << "ERROR: something wrong in serial communication.\n";
	}
}

void USB_Controller::set_accel(char* value) {
	char* p = strtok(value, ".");

	if (p != NULL) {
		accel_values[0] = atoi(p);
	}
	if ((p = strtok(NULL, ".")) != NULL) {
		accel_values[1] = atoi(p);
	}
	if ((p = strtok(NULL, ".")) != NULL) {
		accel_values[2] = atoi(p);
	}
}

void USB_Controller::set_switch_on(char* value) {
	switch (atoi(value)) {
	case 0:
		m_switch_on = false;
		break;
	case 1:
		m_switch_on = true;
		break;
	default:
		cerr << "Error wrong sensor values given: Sensor = switch, Value = "
				<< value << ".\n";
	}
}

void USB_Controller::set_rotation_value(char* value) {
	rotation_value = atoi(value);
}

void USB_Controller::set_potentiometer_value(char* value) {
	potentiometer_value = atoi(value);
}

void USB_Controller::get_accel(int values[3]) {
	for (int i = 0; i < 3; ++i) {
		values[i] = accel_values[i];
	}
}

int USB_Controller::get_accelY() {
	return accel_values[1];
}

bool USB_Controller::switch_on(void) {
	return m_switch_on;
}

int USB_Controller::get_rotation_value(void) {
	return rotation_value;
}

int USB_Controller::get_potentiometer_value(void) {
	return potentiometer_value;
}

void USB_Controller::buzz(int timeout) {
	cout << "Sending: " << HIGH << endl;
	write(fd, HIGH, strlen(HIGH));
}

bool USB_Controller::push_on(void) {
	return push;
}

void USB_Controller::set_push_on(char* value) {
	switch (atoi(value)) {
	case 1:
		push = false;
		break;
	case 0:
		push = true;
		break;
	default:
		cerr << "Error wrong sensor values given: Sensor = push, Value = "
				<< value << ".\n";
	}
}
