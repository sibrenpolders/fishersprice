#ifndef FISHMANAGER_H_
#define FISHMANAGER_H_

#include <irrlicht.h>
#include <iostream>
#include <vector>
#include <string>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

#define DEFAULT_DEPTH_FISH 75.f

typedef enum TYPE_FISH {
	CLOWN, SHARK, MORAY, PERCH
} TYPE_FISH;

typedef struct FishInfo {
	TYPE_FISH type;
	short ID;
	vector3df start;
	vector3df stop;
	vector3df currPos;
	int duration;
	int currMilliSecsPassed;
	bool fromStartToStop;
	IAnimatedMeshSceneNode* node;
	bool isHooked;
	vector3df direction;
} FishInfo;

class FishManager {
public:
	FishManager(IrrlichtDevice *device, IVideoDriver* driver,
			ISceneManager* smgr);
	virtual ~FishManager();
	void reset();

	void setPosition(int ID, vector3df pos);
	vector3df getPosition(int ID);
	void setRotation(int ID, vector3df towardsPoint);
	void releaseHook(int ID);
	int checkForHooking(vector3df posOfHook);
	void update(unsigned long millisecs);
	void addFish(TYPE_FISH fish, vector3df startPos, vector3df endPos,
			int durationMilliSecs);

private:
	IrrlichtDevice * m_device;
	IVideoDriver* m_driver;
	ISceneManager* m_smgr;
	short m_nextID;

	vector<FishInfo*> m_fishes;

	std::string getMeshName(TYPE_FISH fish);
	std::string getTextureName(TYPE_FISH fish);
	core::vector3df getScale(TYPE_FISH fish);
	vector3df calcRotation(vector3df from, vector3df to);
};

#endif
