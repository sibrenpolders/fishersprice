#include "ActionManager.h"

ActionManager::ActionManager(ICameraSceneNode * camera,
		LocationTracker* locationTracker, Cross* cross, FishManager* fishMan) {
	m_camera = camera;
	m_locationTracker = locationTracker;
	m_cross = cross;
	m_fishMan = fishMan;
	reset();
}

ActionManager::~ActionManager() {

}

void ActionManager::reset() {
	m_isHooked = false;
	m_isBroken = false;
	m_isLanded = false;
	m_hookedFishID = -1;

	m_fishMan->reset();
	m_locationTracker->reset();
	m_cross->reset();
}

bool ActionManager::isHooked() {
	bool result = m_isHooked;
	m_isHooked = false;
	return result;
}

bool ActionManager::isBroken() {
	return m_isBroken;
}

bool ActionManager::isLandedWithFish() {
	bool result = m_isLanded && m_hookedFishID != -1;
	m_isLanded = false;
	return result;
}

bool ActionManager::isLandedWithNoFish() {
	bool result = m_isLanded && m_hookedFishID == -1;
	m_isLanded = false;
	return result;
}

bool ActionManager::throwBlocked() {
	bool result = m_throwBlocked;
	m_throwBlocked = false;
	return result;
}

void ActionManager::update(int accelValues[], bool switchOn, int encoderValue,
		int potentioValue, bool pushButtonOn,
		unsigned int lastFrameDurationMilliSeconds) {
	/*usbController.update();
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
	 if (loopNr == 35) {
	 //usbController.buzz(100);
	 cross.swimAway(30);
	 }
	 if (loopNr > 100) {
	 loopNr = 0;
	 //usbController.buzz(100);
	 cross.bringIn(25);
	 }

	 fishMan.update(lastFrameDurationMilliSeconds);*/
}

void ActionManager::goLeft() {
	vector3df currPos = m_camera->getAbsolutePosition();
	currPos.X = currPos.X + 20.f;
	m_camera->setPosition(currPos);
}

void ActionManager::goRight() {
	vector3df currPos = m_camera->getAbsolutePosition();
	currPos.X = currPos.X - 20.f;
	m_camera->setPosition(currPos);
}
