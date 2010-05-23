#include "ActionManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

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
	m_throwBlocked = false;
	m_hasThrownSinceReset = false;
	m_encoderValuesSetWithArduino = false;
	m_hookedFishID = -1;

	m_fishMan->reset();
	m_locationTracker->reset();
	m_cross->reset();
	m_camera->setRotation(vector3df(30, 180, 0));

	m_nextBuzz = 0;
	m_nextSwimAway = 0;

	m_accelY = 0;
	m_switchOn = true;
	m_encoderValue = 0;
	m_prevEncoderValue = 0;
	m_potentioValue = 500;
	m_pushButtonOn = 0;
}

bool ActionManager::isHooked() {
	return m_isHooked;
}

bool ActionManager::isBroken() {
	return m_isBroken;
}

bool ActionManager::isLandedWithFish() {
	bool result = m_isLanded && m_hookedFishID != -1;
	return result;
}

bool ActionManager::isLandedWithNoFish() {
	bool result = m_isLanded && m_hookedFishID == -1;
	return result;
}

bool ActionManager::throwBlocked() {
	return m_throwBlocked;
}

void ActionManager::insertArduinoValues(int accelY, bool switchOn,
		int encoderValue, int potentioValue, bool pushButtonOn) {
	m_accelY = accelY;
	m_switchOn = switchOn;
	m_prevEncoderValue = m_encoderValue;
	m_encoderValue = encoderValue;
	m_potentioValue = potentioValue;
	m_pushButtonOn = pushButtonOn;
	m_encoderValuesSetWithArduino = true;
}

void ActionManager::update(unsigned int lastFrameDurationMilliSeconds,
		unsigned long now) {
	m_fishMan->update(lastFrameDurationMilliSeconds);

	if (m_pushButtonOn)
		reset();
	else if (m_isLanded || m_throwBlocked)
		return;
	else if (!m_hasThrownSinceReset && !m_isBroken) {
		m_locationTracker->insertNewValue(m_accelY, now);
		bool ready = m_locationTracker->newThrowReady();
		if (ready && m_switchOn) {
			m_throwBlocked = true; // line has to be released before throwing it
		} else if (ready && !m_switchOn) {
			m_hasThrownSinceReset = true;
			vector3df coords = m_camera->getAbsolutePosition();

			// A throw takes minimum 100ms.
			unsigned int timeInterval =
					m_locationTracker->getTimeIntervalOfThrow();
			float distance = m_locationTracker->getDistanceIntervalOfThrow()
					* 2500.f * (100.f / timeInterval);
			m_cross->setVisible(true);
			m_cross->setCoords(vector3df(coords.X, 100.f, coords.Z - distance));
		}
	} else if (m_cross->hasLanded())
		m_isLanded = true;
	else if (m_isHooked) {
		if (now > m_nextSwimAway) {
			m_nextSwimAway = now + 30;
			m_cross->swimAway(1);

			vector3df pos = m_cross->getCoords();
			pos.Z -= 50;
			m_fishMan->setPosition(m_hookedFishID, pos);

			vector3df coordsTo = m_camera->getAbsolutePosition();
			coordsTo.Z = -100000; // swimming away
			m_fishMan->setRotation(m_hookedFishID, coordsTo);

			if (pos.getDistanceFrom(m_camera->getAbsolutePosition()) >= 2500) {
				// no more line available -> line is broken
				m_isBroken = true;
				m_isHooked = false;
				m_fishMan->releaseHook(m_hookedFishID);
				m_hookedFishID = -1;
				m_cross->setVisible(true);
			}
		}
		if (m_prevEncoderValue != m_encoderValue && m_isHooked) {
			if (abs(m_prevEncoderValue - m_encoderValue) < m_potentioValue
					/ 100.f || !m_encoderValuesSetWithArduino) {
				m_cross->bringIn(m_prevEncoderValue - m_encoderValue);
				vector3df pos = m_cross->getCoords();
				pos.Z -= 50;
				m_fishMan->setPosition(m_hookedFishID, pos);

				vector3df coordsTo = m_camera->getAbsolutePosition();
				coordsTo.Z = 10000; // bringing in
				m_fishMan->setRotation(m_hookedFishID, coordsTo);
			} else {
				m_isBroken = true;
				m_isHooked = false;
				m_fishMan->releaseHook(m_hookedFishID);
				m_hookedFishID = -1;
				m_cross->setVisible(true);
			}
		}

	} else {
		if (m_prevEncoderValue != m_encoderValue)
			m_cross->bringIn(m_prevEncoderValue - m_encoderValue);

		vector3df pos = m_cross->getCoords();
		m_hookedFishID = m_fishMan->checkForHooking(pos);
		if (m_hookedFishID != -1) {
			m_isHooked = true;
			m_cross->setVisible(true);
			m_fishMan->setPosition(m_hookedFishID, pos);
			vector3df coordsTo = m_camera->getAbsolutePosition();
			coordsTo.Z = -100000; // swimming away
			m_fishMan->setRotation(m_hookedFishID, coordsTo);
		}

	}

	if (!m_encoderValuesSetWithArduino)
		m_prevEncoderValue = m_encoderValue;
}

void ActionManager::goLeft() {
	vector3df currPos = m_camera->getAbsolutePosition();
	currPos.X = currPos.X + 20.f;
	m_camera->setPosition(currPos);
	m_camera->setRotation(vector3df(30, 180, 0));
}

void ActionManager::goRight() {
	vector3df currPos = m_camera->getAbsolutePosition();
	currPos.X = currPos.X - 20.f;
	m_camera->setPosition(currPos);
	m_camera->setRotation(vector3df(30, 180, 0));
}

void ActionManager::toggleSwitch() {
	m_switchOn = !m_switchOn;
}

void ActionManager::throwIn() {
	if (m_switchOn) {
		m_throwBlocked = true; // line has to be released before throwing it
	} else if (!m_switchOn) {
		m_hasThrownSinceReset = true;
		vector3df coords = m_camera->getAbsolutePosition();

		// A throw takes minimum 100ms.
		unsigned int timeInterval = 1.f * (rand() % 500 + 100);
		float distance = 1.0 * 2500.f * (100.f / timeInterval);
		m_cross->setVisible(true);
		m_cross->setCoords(vector3df(coords.X, 100.f, coords.Z - distance));
	}
}

void ActionManager::bringInOneUnit() {
	m_prevEncoderValue = m_encoderValue;
	m_encoderValue -= 10;
}

bool ActionManager::checkBuzz(unsigned long now) {
	if (now >= m_nextBuzz) {
		if (m_isHooked)
			m_nextBuzz = now + 200;
		else
			m_nextBuzz = now + 2000;
		return true;
	} else
		return false;
}
