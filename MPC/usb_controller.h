// author: Sibren Polders

#ifndef __USB_CONTROLLER_H__
#define __USB_CONTROLLER_H__

#include <termios.h>
#include <string>
#include <string.h>
#include "GUIManager.h"
using namespace std;

/* baudrate settings are defined in <asm/termbits.h>, which is
 included by <termios.h> */
#define BAUDRATE B9600

class USB_Controller {
public:
	USB_Controller(GUIManager* guiMan);
	bool init(std::string device_name);
	~USB_Controller();

	// read a string from the serial port and parse it
	int update();

	// getters
	void get_accel(int values[3]);
	int get_accelY();
	bool switch_on(void);
	int get_rotation_value(void);
	int get_potentiometer_value(void);
	bool push_on(void);

	// send buzz command to arduino
	void buzz();
	void nobuzz();
private:
	GUIManager* m_guiMan;

	// member variables
	int accel_values[3];
	bool m_switch_on;
	int rotation_value;
	int potentiometer_value;
	int fd;
	bool push;
	struct termios oldtio, toptions;

	// member consts

	// set the delimiters used by strtok
	const static char* DELIM;
	const static int ACCEL;
	const static int SWITCH;
	const static int ROTATION;
	const static int POTENTIO_METER;
	const static int PUSH;
	const static int SOURCE;
	const static char* BUZZ;
	const static char* NOBUZZ;

	// member functions

	// will parse the string read from the serial port
	void parse(char* string);

	// these functions will set the mmeber values
	void setValue(int substring_number, char* value);
	void set_accel(char* value);
	void set_switch_on(char* value);
	void set_rotation_value(char* value);
	void set_potentiometer_value(char* value);
	void set_push_on(char* value);

	int serialport_write(int fd, const char* str);
	int serialport_read_until(int fd, char* buf, char until);
};
#endif // __USB_CONTROLLER_H__
