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

	// Upperleft
	cout << "Point the top of the rod towards the upperleft corner.\n";
	sleep(5);
	usbController->update();
	usbController->get_accel(values);
	locationTracker->calibrateUpperLeft(values);

	// Upperright
	cout << "Point the top of the rod towards the upperright corner.\n";
	sleep(5);
	usbController->update();
	usbController->get_accel(values);
	locationTracker->calibrateUpperRight(values);

	// Lowerleft
	cout << "Point the top of the rod towards the lowerleft corner.\n";
	sleep(5);
	usbController->update();
	usbController->get_accel(values);
	locationTracker->calibrateLowerLeft(values);

	// Lowerleft
	cout << "Point the top of the rod towards the back of you.\n";
	sleep(5);
	usbController->update();
	usbController->get_accel(values);
	locationTracker->calibrateBackwards(values);

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
	USB_Controller usbController("/dev/ttyUSB0");
	LocationTracker locationTracker;
	locationTracker.setScreenDimensions(800, 600);

	/*
	 cout << "Initiating calibration... .\n";Rebegin... .\n";
	 sleep(10);
	 calibrate(&usbController, &locationTracker);*/

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
			/*usbController.update();
			 int accelValues[3];
			 usbController->get_accel(values);
			 bool switchOn = usbController->switch_on();
			 int encoderValue = usbController->get_rotation_value();
			 int potentioValue = usbController->get_potentiometer_value();
			 bool pushButtonOn = usbController->push_on();

			 actionMan.update(accelValues, switchOn, encoderValue,
			 potentioValue, pushButtonOn, lastFrameDurationMilliSeconds, now);*/
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

			if (actionMan.checkBuzz(now))
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
