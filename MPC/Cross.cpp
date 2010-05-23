#include "Cross.h"
#include <irrlicht.h>
#include <iostream>
#include <string>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

Cross::Cross(irr::video::IVideoDriver* driver, scene::ISceneManager* smgr,
		scene::ITriangleSelector* terrainSelector) {
	m_driver = driver;
	m_smgr = smgr;
	m_coords = vector3df(5725.f, DEFAULT_DEPTH_CROSS, 5655.f);

	// create cube
	m_cube = smgr->addCubeSceneNode(20);
	m_cube->setPosition(m_coords);
	m_cube->setMaterialFlag(video::EMF_LIGHTING, false);
	m_cube->setMaterialTexture(0, driver->getTexture("./media/red.jpg"));

	reset();
}

Cross::~Cross() {

}

void Cross::reset() {
	vector3df ori = m_smgr->getActiveCamera()->getAbsolutePosition();
	ori.Z += 5000.f;
	ori.X += 5000.f;
	ori.Y = DEFAULT_DEPTH_CROSS - 10000.f;
	setCoords(ori);

	setVisible(true);
	m_lastMoveWasAway = false;
}

void Cross::setCoords(vector3df coords) {
	if (coords.Y < DEFAULT_DEPTH_CROSS)
		coords.Y = DEFAULT_DEPTH_CROSS;
	this->m_coords = coords;
	m_cube->setPosition(m_coords);
}

vector3df Cross::getCoords() {
	return m_cube->getAbsolutePosition();
}

void Cross::setVisible(bool visible) {
	m_visible = visible;
	m_cube->setVisible(visible);
}

void Cross::bringIn(int nbTicksOfReel) {
	vector3df ori = m_smgr->getActiveCamera()->getPosition();
	m_coords = m_cube->getAbsolutePosition();

	double distance = ori.getDistanceFrom(m_coords);
	double distanceToTravel = 5 * nbTicksOfReel;
	double ratio = distanceToTravel / distance;

	vector3df newPosition = ori + (1.0f - ratio) * (m_coords - ori);

	if (ori.getDistanceFrom(newPosition) >= 200.f)
		newPosition.Y = DEFAULT_DEPTH_CROSS;
	setCoords(newPosition);
	m_lastMoveWasAway = false;
}

void Cross::swimAway(int nbUnits) {
	vector3df ori = m_smgr->getActiveCamera()->getPosition();
	m_coords = m_cube->getAbsolutePosition();

	double distance = ori.getDistanceFrom(m_coords);
	double distanceToTravel = 5 * nbUnits;
	double ratio = distanceToTravel / distance;

	vector3df newPosition = ori + (1.0f + ratio) * (m_coords - ori);

	if (ori.getDistanceFrom(newPosition) >= 500.f)
		newPosition.Y = DEFAULT_DEPTH_CROSS;
	setCoords(newPosition);
	m_lastMoveWasAway = true;
}

bool Cross::hasLanded() {
	if (!m_lastMoveWasAway) {
		vector3df ori = m_smgr->getActiveCamera()->getPosition();
		m_coords = m_cube->getAbsolutePosition();
		double distance = ori.getDistanceFrom(m_coords);

		return distance <= 50.f;
	} else
		return false;
}
