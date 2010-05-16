#ifndef CROSS_H_
#define CROSS_H_

#include <irrlicht.h>
#include <iostream>
#include <string>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

class Cross {
public:
	Cross(irr::video::IVideoDriver* driver, scene::ISceneManager* smgr,
			scene::ITriangleSelector* terrainSelector);
	virtual ~Cross();
	void reset();

	void setCoords(vector3df coords); // for when thrown in
	void setVisible(bool visible);

	void bringIn(int nbTicksOfReel);
	void swimAway(int nbUnits);
	bool hasBroken();
	bool hasLanded();

private:
	bool m_visible;
	irr::video::IVideoDriver* m_driver;
	scene::ISceneManager* m_smgr;
	vector3df m_coords;
	scene::ISceneNode* m_cube;

	bool m_lastMoveWasAway;
};

#endif /* CROSS_H_ */
