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
	m_coords = vector3df(5725.f, 100.f, 5655.f);

	// create cube
	m_cube = smgr->addCubeSceneNode(20);
	m_cube->setPosition(m_coords);
	m_cube->setMaterialFlag(video::EMF_LIGHTING, false);
	m_cube->setMaterialTexture(0, driver->getTexture("./media/red.jpg"));

	scene::ISceneNodeAnimator* anim = m_smgr->createCollisionResponseAnimator(
			terrainSelector, m_cube);
	m_cube->addAnimator(anim);
	anim->drop();

	reset();
}

Cross::~Cross() {

}

void Cross::reset() {
	setVisible(false);
	m_lastMoveWasAway = false;
}

void Cross::setCoords(vector3df coords) {
	this->m_coords = coords;
	m_cube->setPosition(m_coords);
	setVisible(true);
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
	ori.Y = 0.f;
	float Y = m_cube->getAbsolutePosition().Y;
	m_coords = m_cube->getAbsolutePosition();
	m_coords.Y = 0.f;
	double distance = ori.getDistanceFrom(m_coords);
	double distanceToTravel = 5 * nbTicksOfReel;
	double ratio = distanceToTravel / distance;

	vector3df newPosition = ori + (1.0f - ratio) * (m_coords - ori);
	newPosition.Y = Y;
	setCoords(newPosition);
	m_lastMoveWasAway = false;
}

void Cross::swimAway(int nbUnits) {
	vector3df ori = m_smgr->getActiveCamera()->getPosition();
	ori.Y = 0.f;
	float Y = m_cube->getAbsolutePosition().Y;
	m_coords = m_cube->getAbsolutePosition();
	m_coords.Y = 0.f;
	double distance = ori.getDistanceFrom(m_coords);
	double distanceToTravel = 5 * nbUnits;
	double ratio = distanceToTravel / distance;

	vector3df newPosition = ori + (1.0f + ratio) * (m_coords - ori);
	newPosition.Y = Y;
	setCoords(newPosition);
	m_lastMoveWasAway = true;
}

bool Cross::hasLanded() {
	if (!m_lastMoveWasAway) {
		vector3df ori = m_smgr->getActiveCamera()->getPosition();
		ori.Y = 0.f;
		float Y = m_cube->getAbsolutePosition().Y;
		m_coords = m_cube->getAbsolutePosition();
		m_coords.Y = 0.f;
		double distance = ori.getDistanceFrom(m_coords);
		m_coords.Y = Y;

		return distance <= 50.f;
	} else
		return false;
}
