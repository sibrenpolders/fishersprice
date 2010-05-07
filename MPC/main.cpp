#include <irrlicht.h>
#include <iostream>
#include <string>
#include "SceneManager.h"
#include "LocationTracker.h"
#include "FishManager.h"
#include "usb_controller.h"
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

	sleep(10);
	calibrate(&usbController, &locationTracker);

	//Using opengl, Screen size 800, 600, 32 bit, fullscreen = false, stencilbuffer = true, vsync = false
	IrrlichtDevice *device = createDevice(EDT_OPENGL, core::dimension2d<u32>(
			800, 600), 16, false, true, false);
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	SceneManager sceneMan(device, driver, smgr);
	ICameraSceneNode * camera = sceneMan.init();

	FishManager fishMan(device, driver, smgr);
	fishMan.addFish(SHARK, vector3df(5725.f, 75.f, 5655.f), vector3df(3635.f,
			75.f, 6235.f), 15000);
	fishMan.addFish(CLOWN, vector3df(3387.f, 75.f, 6024.f), vector3df(4350.f,
			75.f, 3750.f), 35000);

	/////////////////////////////////////////////////////////////////////////
	//                             Main loop
	/////////////////////////////////////////////////////////////////////////
	int lastFPS = -1;
	u32 then = device->getTimer()->getTime();
	u32 now = device->getTimer()->getTime();
	int loopNr = 0;

	while (device->run())
		if (device->isWindowActive()) {
			now = device->getTimer()->getTime();
			unsigned int lastFrameDurationMilliSeconds = now - then;
			then = now;

			usbController.update();
			printUSBControllerValues(&usbController);
			int values[3];
			locationTracker.insertNewValues(values, now);
			if (locationTracker.newThrowReady()) {
				unsigned int interval =
						locationTracker.getTimeIntervalOfThrow();
				float distance = locationTracker.getDistanceIntervalOfThrow();
				int* posInPlane = locationTracker.getPositionInPlane(values);
				int horX = camera->getAbsolutePosition().X;
				int horY = camera->getAbsolutePosition().Y;

				//	int* droppedPoint = lineThrower.throwLine(distance, interval,
				//		posinPlane, horX, horY);
				//	drawCross(droppedPoint);
			}

			loopNr++;
			if (loopNr > 20) {
				loopNr = 0;
				//usbController.buzz(100);
			}

			fishMan.update(lastFrameDurationMilliSeconds);

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
