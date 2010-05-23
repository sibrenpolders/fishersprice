#include <irrlicht.h>
#include <iostream>
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

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

void calibrate(USB_Controller* usbController, LocationTracker* locationTracker,
		GUIManager* guiMan) {
	int values[3];

	// back
	guiMan->setText(GUI_ID_GAME_MESSAGE,
			"Point the top of the rod to the ground behind you.\n");
	sleep(8);
	usbController->update();
	locationTracker->calibrateBack(usbController->get_accelY());

	// front
	guiMan->setText(GUI_ID_GAME_MESSAGE,
			"Point the top of the rod to the ground in front of you.\n");
	sleep(8);
	usbController->update();
	locationTracker->calibrateFront(usbController->get_accelY());

	locationTracker->reset();
}

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

int main() {
	srand(time(NULL));

	IrrlichtDevice *device = createDevice(EDT_OPENGL, core::dimension2d<u32>(
			1280, 800), 16, true, true, false);
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	KeyEventReceiver receiver;
	device->setEventReceiver(&receiver);
	SceneManager sceneMan(device, driver, smgr);
	ICameraSceneNode * camera = sceneMan.init();
	Cross cross(driver, smgr, sceneMan.getTerrainSelector());
	FishManager fishMan(device, driver, smgr);
	fishMan.addFish(SHARK, vector3df(5725.f, 75.f, 5655.f), vector3df(3635.f,
			75.f, 6235.f), 15000);
	fishMan.addFish(CLOWN, vector3df(3387.f, 75.f, 6024.f), vector3df(4350.f,
			75.f, 3750.f), 35000);
	GUIManager guiMan(device);
	LocationTracker locationTracker;
	ActionManager actionMan(camera, &locationTracker, &cross, &fishMan);
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
	u32 then = device->getTimer()->getTime();
	u32 now = device->getTimer()->getTime();

	while (device->run() && !receiver.getStopApp())
		if (device->isWindowActive()) {
			now = device->getTimer()->getTime();
			unsigned int lastFrameDurationMilliSeconds = now - then;
			then = now;

			// retrieve serial values
			if (arduinoInputAvailable && usbController.update() > 4) {
				printUSBControllerValues(&usbController, &guiMan);
				int accelY = usbController.get_accelY();
				bool switchOn = usbController.switch_on();
				int encoderValue = usbController.get_rotation_value();
				int potentioValue = usbController.get_potentiometer_value();
				bool pushButtonOn = usbController.push_on();

				actionMan.insertArduinoValues(accelY, switchOn, encoderValue,
						potentioValue, pushButtonOn);
			}
			printUSBControllerValues(&usbController, &guiMan);

			actionMan.update(lastFrameDurationMilliSeconds, now);

			if (actionMan.isHooked()) {
				guiMan.setText(GUI_ID_GAME_MESSAGE,
						"Gotcha !\nA fish has been hooked, bring it in!");
			}
			if (actionMan.isBroken()) {
				guiMan.setText(GUI_ID_GAME_MESSAGE,
						"Oh no, your line has broken, don't be so greedy!\nRebegin/reset... .\n");
			}
			if (actionMan.isLandedWithFish()) {
				guiMan.setText(GUI_ID_GAME_MESSAGE,
						"Yay, the fish has been landed!\nRebegin/reset... .\n");
			}
			if (actionMan.isLandedWithNoFish()) {
				guiMan.setText(GUI_ID_GAME_MESSAGE,
						"No more line to bring in.\nRebegin/reset... .\n");
			}
			if (actionMan.throwBlocked()) {
				guiMan.setText(
						GUI_ID_GAME_MESSAGE,
						"Oh no, your line was blocked while throwing!\nRelease the line before throwing! Rebegin/reset... .\n");
			}

			if (actionMan.checkBuzz(now) && arduinoInputAvailable)
				usbController.buzz(100);

			/*cout << "CAM: " << camera->getAbsolutePosition().X << " : "
			 << camera->getAbsolutePosition().Y << " : "
			 << camera->getAbsolutePosition().Z << endl;*/

			driver->beginScene(true, true, video::SColor(0, 200, 200, 200));
			smgr->drawAll();
			guienv->drawAll();
			driver->endScene();

			//Shows FPS in the title screen
			int fps = driver->getFPS();
			if (lastFPS != fps) {
				core::stringw str = L"Fisher's Price [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;
				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}
		} else {
			device->yield();
		}
		device->drop();
		return 0;
	}
