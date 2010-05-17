#include "ActionManager.h"

ActionManager::ActionManager(ICameraSceneNode * camera,
		LocationTracker* locationTracker, Cross* cross, FishManager* fishMan) {
	m_camera = camera;
	m_locationTracker = locationTracker;
	m_cross = cross;
	m_fishMan = fishMan;
	m_accelValues = new int[3];
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
	m_hookedFishID = -1;

	m_fishMan->reset();
	m_locationTracker->reset();
	m_cross->reset();
	m_camera->setRotation(vector3df(30, 180, 0));

	m_nextBuzz = 0;
	m_nextSwimAway = 0;
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

void ActionManager::update(int accelValues[], bool switchOn, int encoderValue,
		int potentioValue, bool pushButtonOn,
		unsigned int lastFrameDurationMilliSeconds, unsigned long now) {
	memcpy(m_accelValues, accelValues, 3 * sizeof(int));
	m_switchOn = switchOn;
	int prevEncoderValue = m_encoderValue;
	m_encoderValue = encoderValue;
	m_potentioValue = potentioValue;
	m_pushButtonOn = pushButtonOn;

	m_fishMan->update(lastFrameDurationMilliSeconds);

	if (m_pushButtonOn)
		reset();
	else if (m_isBroken || m_isLanded || m_throwBlocked)
		return;
	else if (!m_hasThrownSinceReset) {
		m_locationTracker->insertNewValues(m_accelValues, now);
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
		if (prevEncoderValue != encoderValue) {
			if ((prevEncoderValue - encoderValue) < potentioValue / 200.f) {
				m_cross->bringIn(prevEncoderValue - encoderValue);
				vector3df pos = m_cross->getCoords();
				m_fishMan->setPosition(m_hookedFishID, pos);

				vector3df coordsTo = m_camera->getAbsolutePosition();
				coordsTo.Z = 0; // bringing in
				m_fishMan->setRotation(m_hookedFishID, coordsTo);
			} else {
				m_isBroken = true;
				m_isHooked = false;
				m_fishMan->releaseHook(m_hookedFishID);
				m_hookedFishID = -1;
				m_cross->setVisible(true);
			}
		}

		if (now > m_nextSwimAway) {
			m_nextSwimAway = now + 30;
			m_cross->swimAway(1);

			vector3df pos = m_cross->getCoords();
			m_fishMan->setPosition(m_hookedFishID, pos);

			vector3df coordsTo = m_camera->getAbsolutePosition();
			coordsTo.Z = 10000; // swimming away
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
	} else {
		vector3df pos = m_cross->getCoords();
		m_hookedFishID = m_fishMan->checkForHooking(pos);
		if (m_hookedFishID != -1) {
			m_isHooked = true;
			m_cross->setVisible(false);
			m_fishMan->setPosition(m_hookedFishID, pos);
			vector3df coordsTo = m_camera->getAbsolutePosition();
			coordsTo.Z = 10000; // swimming away
			m_fishMan->setRotation(m_hookedFishID, coordsTo);
		}
	}
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
