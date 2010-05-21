#ifndef ACTIONMANAGER_H_
#define ACTIONMANAGER_H_

#include <irrlicht.h>
#include "LocationTracker.h"
#include "FishManager.h"
#include "Cross.h"
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

class ActionManager {
public:
	ActionManager(ICameraSceneNode * camera, LocationTracker* locationTracker,
			Cross* cross, FishManager* fishMan);
	virtual ~ActionManager();
	void reset();

	void insertArduinoValues(int accelY, bool switchOn, int encoderValue,
			int potentioValue, bool pushButtonOn);

	void update(unsigned int lastFrameDurationMilliSeconds, unsigned long now);

	bool isHooked();
	bool isBroken();
	bool isLandedWithFish();
	bool isLandedWithNoFish();
	bool throwBlocked();

	bool checkBuzz(unsigned long now);

	// For keyboard input
	void goLeft();
	void goRight();
	void toggleSwitch();
	void throwIn();
	void bringInOneUnit();
private:
	bool m_isHooked;
	bool m_isBroken;
	bool m_isLanded;
	bool m_throwBlocked;
	int m_hookedFishID;

	int m_accelY;
	bool m_switchOn;
	int m_encoderValue;
	int m_prevEncoderValue;
	int m_potentioValue;
	bool m_pushButtonOn;
	bool m_hasThrownSinceReset;

	bool m_encoderValuesSetWithArduino;

	unsigned long m_nextBuzz;
	unsigned long m_nextSwimAway;

	ICameraSceneNode * m_camera;
	LocationTracker* m_locationTracker;
	Cross* m_cross;
	FishManager* m_fishMan;
};

#endif /* ACTIONMANAGER_H_ */
