#include <irrlicht.h>
#include <iostream>
#include <string>
#include "SceneManager.h"
#include "LocationTracker.h"
#include "FishManager.h"
#include "ActionManager.h"
#include "usb_controller.h"
#include "KeyEventReceiver.h"
#include "Cross.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

void calibrate(USB_Controller* usbController, LocationTracker* locationTracker) {
	int values[3];

	// back
	cout << "Point the top of the rod to the ground behind you.\n";
	sleep(8);
	usbController->update();
	locationTracker->calibrateBack(usbController->get_accelY());

	// front
	cout << "Point the top of the rod to the ground in front of you.\n";
	sleep(8);
	usbController->update();
	locationTracker->calibrateFront(usbController->get_accelY());

	locationTracker->reset();
}

void printUSBControllerValues(USB_Controller* usbController) {
	int values[3];
	usbController->get_accel(values);
	cout << "Accel: ";
	for (int i = 0; i < 3; ++i) {
		cout << values[i] << "  ";
	}
	cout << endl;
	cout << "Switch: " << usbController->switch_on() << endl;
	cout << "Encoder: " << usbController->get_rotation_value() << endl;
	cout << "Potention: " << usbController->get_potentiometer_value() << endl;
	cout << "Push: " << usbController->push_on() << endl << endl;
}

int main() {
	srand(time(NULL));

	USB_Controller usbController;
	bool arduinoInputAvailable = usbController.init("/dev/ttyUSB0");
	LocationTracker locationTracker;

	if (arduinoInputAvailable) {
		cout << "Initiating calibration... .\n";
		sleep(5);
		calibrate(&usbController, &locationTracker);
	}

	IrrlichtDevice *device = createDevice(EDT_OPENGL, core::dimension2d<u32>(
			800, 600), 16, false, true, false);
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
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
	ActionManager actionMan(camera, &locationTracker, &cross, &fishMan);
	receiver.setActionManager(&actionMan);

	/////////////////////////////////////////////////////////////////////////
	//                             Main loop
	/////////////////////////////////////////////////////////////////////////
	int lastFPS = -1;
	u32 then = device->getTimer()->getTime();
	u32 now = device->getTimer()->getTime();
	int loopNr = 0;

	while (device->run() && !receiver.getStopApp())
		if (device->isWindowActive()) {
			now = device->getTimer()->getTime();
			unsigned int lastFrameDurationMilliSeconds = now - then;
			then = now;

			// retrieve serial values
			if (arduinoInputAvailable && usbController.update() > 4) {
				printUSBControllerValues(&usbController);
				int accelY = usbController.get_accelY();
				bool switchOn = usbController.switch_on();
				int encoderValue = usbController.get_rotation_value();
				int potentioValue = usbController.get_potentiometer_value();
				bool pushButtonOn = usbController.push_on();

				actionMan.insertArduinoValues(accelY, switchOn, encoderValue,
						potentioValue, pushButtonOn);
			}

			actionMan.update(lastFrameDurationMilliSeconds, now);

			if (actionMan.isHooked()) {
				cout << "A fish has been hooked, bring it in!\n";
			}
			if (actionMan.isBroken()) {
				cout << "Oh no, your line has broken, don't be so greedy!\n";
				cout << "Rebegin/reset... .\n";
			}
			if (actionMan.isLandedWithFish()) {
				cout << "Yay, the fish has been landed!";
				cout << "Rebegin/reset... .\n";
			}
			if (actionMan.isLandedWithNoFish()) {
				cout << "No more line to bring in.\n";
				cout << "Rebegin/reset... .\n";
			}
			if (actionMan.throwBlocked()) {
				cout << "Oh no, your line was blocked while throwing!\n";
				cout
						<< "Release the line before throwing! Rebegin/reset... .\n";
			}

			if (actionMan.checkBuzz(now) && arduinoInputAvailable)
				usbController.buzz(100);

			/*cout << "CAM: " << camera->getAbsolutePosition().X << " : "
			 << camera->getAbsolutePosition().Y << " : "
			 << camera->getAbsolutePosition().Z << endl;*/

			driver->beginScene(true, true, video::SColor(0, 200, 200, 200));
			smgr->drawAll();
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