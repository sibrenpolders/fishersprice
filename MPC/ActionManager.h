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

	void update(int accelValues[], bool switchOn, int encoderValue,
			int potentioValue, bool pushButtonOn,
			unsigned int lastFrameDurationMilliSeconds);

	bool isHooked();
	bool isBroken();
	bool isLandedWithFish();
	bool isLandedWithNoFish();
	bool throwBlocked();

	void goLeft();
	void goRight();
private:
	bool m_isHooked;
	bool m_isBroken;
	bool m_isLanded;
	bool m_throwBlocked;
	int m_hookedFishID;

	int m_accelValues[3];
	bool m_switchOn;
	int m_encoderValue;
	bool m_pushButtonOn;

	ICameraSceneNode * m_camera;
	LocationTracker* m_locationTracker;
	Cross* m_cross;
	FishManager* m_fishMan;
};

#endif /* ACTIONMANAGER_H_ */
