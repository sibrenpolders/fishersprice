// author: Sibrand Staessens

#ifndef CROSS_H_
#define CROSS_H_

// Klasse die de dobber voorstelt en ook niet meer dan dat.
// Zie ActionManager voor het geheel.

#include <irrlicht.h>
#include <iostream>
#include <string>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

#define DEFAULT_DEPTH_CROSS 100.f

class Cross {
public:
	Cross(irr::video::IVideoDriver* driver, scene::ISceneManager* smgr,
			scene::ITriangleSelector* terrainSelector);
	virtual ~Cross();
	void reset();

	void setCoords(vector3df coords); // for when to throw in
	void setVisible(bool visible);
	vector3df getCoords();

	void bringIn(int nbTicksOfReel); // related to serial input
	void swimAway(int nbUnits); // related to the power of the fish
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
