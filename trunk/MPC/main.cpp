#include <irrlicht.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "SceneManager.h"
#include "LocationTracker.h"
#include "FishManager.h"
#include "ActionManager.h"
#include "GUIManager.h"
#include "usb_controller.h"
#include "KeyEventReceiver.h"
#include "Cross.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

#define FILENAME "out.csv"
#define TIMEOUT_FOR_CALIBRATION 8000
typedef enum GAME_STAGE {
	CALIBRATE_BACK = 0, CALIBRATE_FRONT, IN_GAME
} GAME_STAGE;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

std::string intToString(int val) {
	std::ostringstream stm;
	stm << val;
	return stm.str();
}

void printUSBControllerValues(USB_Controller* usbController,
		GUIManager* guiManager) {
	guiManager->setText(GUI_ID_ACCEL, intToString(usbController->get_accelY()));
	guiManager->setText(GUI_ID_POT, intToString(
			usbController->get_potentiometer_value()));
	guiManager->setText(GUI_ID_ENCODER, intToString(
			usbController->get_rotation_value()));

	if (usbController->switch_on())
		guiManager->setText(GUI_ID_SWITCH, std::string("on"));
	else
		guiManager->setText(GUI_ID_SWITCH, std::string("off"));

	if (usbController->push_on())
		guiManager->setText(GUI_ID_PUSH, std::string("on"));
	else
		guiManager->setText(GUI_ID_PUSH, std::string("off"));
}

void resetFile() {
	ofstream myfile;
	myfile.open(FILENAME, ios::out | ios::trunc);
	myfile
			<< "Timestamp,Accelerometer,Encoder,Potentiometer,Pushbutton,Switch\n";
	myfile.close();
}

void addUSBControllerValuesToFile(USB_Controller* usbController,
		unsigned long now) {
	ofstream myfile;
	myfile.open(FILENAME, ios::out | ios::app);
	myfile << now << ",";
	myfile << usbController->get_accelY() << ",";
	myfile << usbController->get_rotation_value() << ",";
	myfile << usbController->get_potentiometer_value() << ",";
	myfile << usbController->push_on() << ",";
	myfile << usbController->switch_on() << "\n";
	myfile.close();
}

int main() {
	srand(time(NULL));

	IrrlichtDevice *pdevice = createDevice(EDT_OPENGL, core::dimension2d<u32>(
			SCREEN_WIDTH, SCREEN_HEIGTH), 16, true, true, false);
	video::IVideoDriver* pdriver = pdevice->getVideoDriver();
	scene::ISceneManager* psmgr = pdevice->getSceneManager();
	gui::IGUIEnvironment* pguienv = pdevice->getGUIEnvironment();

	KeyEventReceiver receiver;
	pdevice->setEventReceiver(&receiver);
	SceneManager sceneMan(pdevice, pdriver, psmgr);
	ICameraSceneNode * pcamera = sceneMan.init();
	Cross cross(pdriver, psmgr, sceneMan.getTerrainSelector());
	FishManager fishMan(pdevice, pdriver, psmgr);
	fishMan.addFish(SHARK, vector3df(5725.f, DEFAULT_DEPTH_FISH, 5655.f),
			vector3df(3635.f, DEFAULT_DEPTH_FISH, 6235.f), 15000);
	fishMan.addFish(CLOWN, vector3df(3387.f, DEFAULT_DEPTH_FISH, 6024.f),
			vector3df(4350.f, DEFAULT_DEPTH_FISH, 3750.f), 35000);
	GUIManager guiMan(pdevice);
	LocationTracker locationTracker;
	ActionManager actionMan(pcamera, &locationTracker, &cross, &fishMan);
	receiver.setActionManager(&actionMan);

	USB_Controller usbController(&guiMan);
	bool arduinoInputAvailable = usbController.init("/dev/ttyUSB0");

	if (arduinoInputAvailable) {
		guiMan.setText(GUI_ID_GAME_MESSAGE, "Initiating calibration... .\n");
		//sleep(5);
		//calibrate(&usbController, &locationTracker, &guiMan);
	}

	/////////////////////////////////////////////////////////////////////////
	//                             Main loop
	/////////////////////////////////////////////////////////////////////////
	int lastFPS = -1;
	u32 then = pdevice->getTimer()->getTime();
	u32 now = pdevice->getTimer()->getTime();
	u32 timeout = now + TIMEOUT_FOR_CALIBRATION;

	int accelY;
	bool switchOn;
	int encoderValue;
	int potentioValue;
	bool pushButtonOn;

	GAME_STAGE stage = CALIBRATE_BACK;
	printUSBControllerValues(&usbController, &guiMan);
	resetFile();
	actionMan.reset();

	while (pdevice->run() && !receiver.getStopApp())
		if (pdevice->isWindowActive()) {
			now = pdevice->getTimer()->getTime();
			unsigned int lastFrameDurationMilliSeconds = now - then;
			then = now;

			// retrieve serial values
			if (arduinoInputAvailable && usbController.update() > 8) {
				printUSBControllerValues(&usbController, &guiMan);
				addUSBControllerValuesToFile(&usbController, now);
			}

			switch (stage) {
			case CALIBRATE_BACK:
				guiMan.setText(GUI_ID_GAME_MESSAGE,
						"Point the top of the rod to the ground behind you.\n");

				if (now > timeout) {
					timeout = now + TIMEOUT_FOR_CALIBRATION;
					stage = CALIBRATE_FRONT;
					if (arduinoInputAvailable)
						locationTracker.calibrateBack(
								usbController.get_accelY());
				}
				break;
			case CALIBRATE_FRONT:
				guiMan.setText(GUI_ID_GAME_MESSAGE,
						"Point the top of the rod to the ground in front of you.\n");

				if (now > timeout) {
					timeout = now + TIMEOUT_FOR_CALIBRATION;
					stage = IN_GAME;
					if (arduinoInputAvailable)
						locationTracker.calibrateFront(
								usbController.get_accelY());
					locationTracker.reset();
					actionMan.reset();
				}
				break;
			case IN_GAME:
				if (arduinoInputAvailable) {
					accelY = usbController.get_accelY();
					switchOn = usbController.switch_on();
					encoderValue = usbController.get_rotation_value();
					potentioValue = usbController.get_potentiometer_value();
					pushButtonOn = usbController.push_on();

					actionMan.insertArduinoValues(accelY, switchOn,
							encoderValue, potentioValue, pushButtonOn);
				}
				actionMan.update(lastFrameDurationMilliSeconds, now);

				if (actionMan.isHooked()) {
					guiMan.setText(GUI_ID_GAME_MESSAGE,
							"Gotcha !\nA fish has been hooked, bring it in!");
				} else if (actionMan.isBroken()) {
					guiMan.setText(GUI_ID_GAME_MESSAGE,
							"Oh no, your line has broken, don't be so greedy!\nRebegin/reset... .\n");
				} else if (actionMan.isLandedWithFish()) {
					guiMan.setText(GUI_ID_GAME_MESSAGE,
							"Yay, the fish has been landed!\nRebegin/reset... .\n");
				} else if (actionMan.isLandedWithNoFish()) {
					guiMan.setText(GUI_ID_GAME_MESSAGE,
							"No more line to bring in.\nRebegin/reset... .\n");
				} else if (actionMan.throwBlocked()) {
					guiMan.setText(
							GUI_ID_GAME_MESSAGE,
							"Oh no, your line was blocked while throwing!\nRelease the line before throwing! Rebegin/reset... .\n");
				} else
					guiMan.setText(GUI_ID_GAME_MESSAGE, "Happy fishing! :)\n");

				if (actionMan.checkBuzz(now) && arduinoInputAvailable)
					usbController.buzz(100);
				break;
			default:
				cerr << "Unknown game stage.\n";
				break;
			}

			/*cout << "CAM: " << camera->getAbsolutePosition().X << " : "
			 << camera->getAbsolutePosition().Y << " : "
			 << camera->getAbsolutePosition().Z << endl;*/

			pdriver->beginScene(true, true, video::SColor(0, 200, 200, 200));
			psmgr->drawAll();
			pguienv->drawAll();
			pdriver->endScene();

			//Shows FPS in the title screen
			int fps = pdriver->getFPS();
			if (lastFPS != fps) {
				core::stringw str = L"Fisher's Price [";
				str += pdriver->getName();
				str += "] FPS:";
				str += fps;
				pdevice->setWindowCaption(str.c_str());
				lastFPS = fps;
			}
		} else {
			pdevice->yield();
		}
		pdevice->drop();
		return 0;
	}
