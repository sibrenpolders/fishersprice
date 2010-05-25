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

const char* USB_Controller::DELIM = ":";
const int USB_Controller::SWITCH = 0;
const int USB_Controller::PUSH = 1;
const int USB_Controller::ROTATION = 2;
const int USB_Controller::POTENTIO_METER = 3;
const int USB_Controller::ACCEL = 4;
const int USB_Controller::SOURCE = 5;
const char* USB_Controller::BUZZ = "C";
const char* USB_Controller::NOBUZZ = "N";

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
	fd = open(device_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		perror("init_serialport: Unable to open port ");
		return -1;
	}

	if (tcgetattr(fd, &toptions) < 0) {
		perror("init_serialport: Couldn't get term attributes");
		return -1;
	}

	cfsetispeed(&toptions, BAUDRATE);
	cfsetospeed(&toptions, BAUDRATE);

	// 8N1
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	// no flow control
	toptions.c_cflag &= ~CRTSCTS;

	toptions.c_cflag |= CREAD | CLOCAL; // turn on READ & ignore ctrl lines
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	toptions.c_oflag &= ~OPOST; // make raw

	// see: http://unixwiz.net/techtips/termios-vmin-vtime.html
	toptions.c_cc[VMIN] = 0;
	toptions.c_cc[VTIME] = 20;

	if (tcsetattr(fd, TCSANOW, &toptions) < 0) {
		perror("init_serialport: Couldn't set term attributes");
		return false;
	}

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
	serialport_read_until(fd, buf, '\n');
	int res = strlen(buf);
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
		;//cerr << "ERROR: something wrong in serial communication.\n";
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
		;//cerr << "ERROR: something wrong in serial communication.\n";
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
		;//cerr << "Error wrong sensor values given: Sensor = switch, Value = "	<< value << ".\n";
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

void USB_Controller::buzz() {
	serialport_write(fd, "C");
}

void USB_Controller::nobuzz() {
	serialport_write(fd, "N");
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
		;//cerr << "Error wrong sensor values given: Sensor = push, Value = " << value << ".\n";
	}
}

int USB_Controller::serialport_read_until(int fd, char* buf, char until) {
	char b[1];
	int i = 0;
	do {
		int n = read(fd, b, 1); // read a char at a time
		if (n == -1)
			return -1; // couldn't read
		if (n == 0) {
			usleep(10 * 1000); // wait 10 msec try again
			continue;
		}
		buf[i] = b[0];
		i++;
	} while (b[0] != until);

	buf[i] = 0; // null terminate the string
	return 0;
}

int USB_Controller::serialport_write(int fd, const char* str) {
	int len = strlen(str);
	int n = write(fd, str, len);

	if (n != len)
		return -1;
	return 0;
}
