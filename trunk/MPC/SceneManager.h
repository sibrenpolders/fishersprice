// author: Sibrand Staessens

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

#define GRAVITY_ON false

class SceneManager {
public:
	SceneManager(IrrlichtDevice *device, IVideoDriver* driver,
			ISceneManager* smgr);
	virtual ~SceneManager();
	ICameraSceneNode * init();
	scene::ITriangleSelector* getTerrainSelector() {
		return m_terrainSelector;
	}

private:
	void initCamera();
	void loadWater();
	void loadTerrain();
	void loadSky();

	IrrlichtDevice *m_device;
	IVideoDriver* m_driver;
	ISceneManager* m_smgr;
	ICameraSceneNode * m_camera;
	scene::ITriangleSelector* m_terrainSelector;
};

#endif
