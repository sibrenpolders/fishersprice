// author: Sibrand Staessens

#include "SceneManager.h"

SceneManager::SceneManager(IrrlichtDevice *device, IVideoDriver* driver,
		ISceneManager* smgr) {
	this->m_device = device;
	this->m_driver = driver;
	this->m_smgr = smgr;
	this->m_camera = 0;
}

SceneManager::~SceneManager() {

}

ICameraSceneNode * SceneManager::init() {
	m_smgr->setAmbientLight(video::SColorf(0.3, 0.3, 0.3, 1.0));

	initCamera();
	loadTerrain();
	loadSky();
	loadWater();

	return m_camera;
}

void SceneManager::initCamera() {
	//parent = 0, rotateSpeed AKA mouse = 100, moveSpeed = 0.8f
	m_camera = m_smgr->addCameraSceneNodeFPS(0, 100.f, 1.0f);
	m_camera->setPosition(core::vector3df(3675.f, 270.f, 6500.f));
	m_device->getCursorControl()->setVisible(false);
	m_camera->setFarValue(10000);
	m_camera->setRotation(vector3df(30.f, 180.f, 0));
}

void SceneManager::loadWater() {
	//Create mesh to add water effect to
	IAnimatedMesh* plane = m_smgr->addHillPlaneMesh("plane", // Name of mesh
			core::dimension2d<f32>(20, 20), //	Size of a tile of the mesh.
			core::dimension2d<u32>(200, 150), 0, 0, // 	Specifies how much tiles there will be.
			core::dimension2d<f32>(0, 0), // countHills
			core::dimension2d<f32>(250, 250)); // textureRepeatCount

	//mesh, waveheight, wave speed, wave length
	ISceneNode* sea = m_smgr->addWaterSurfaceSceneNode(plane->getMesh(0), 1.0f,
			300.0f, 0.5f);
	sea->setMaterialTexture(0, m_driver->getTexture("./media/stones.jpg"));
	sea->setMaterialTexture(1, m_driver->getTexture("./media/water.jpg"));
	sea->setMaterialFlag(EMF_LIGHTING, false);
	sea->setMaterialType(video::EMT_TRANSPARENT_REFLECTION_2_LAYER);
	sea->setPosition(vector3df(5000, 100, 5000));
}

void SceneManager::loadTerrain() {
	scene::ITerrainSceneNode* terrain = m_smgr->addTerrainSceneNode(
			"./media/terrain-heightmap2.bmp", 0, // parent node
			-1, // node id
			core::vector3df(0.f, 0.f, 0.f), // position
			core::vector3df(0.f, 0.f, 0.f), // rotation
			core::vector3df(40.f, 4.4f, 40.f), // scale
			video::SColor(255, 255, 255, 255), // vertexColor
			5, // maxLOD
			scene::ETPS_17, // patchSize
			10// smoothFactor
			);

	terrain->setMaterialFlag(video::EMF_LIGHTING, false);
	terrain->setMaterialTexture(0, m_driver->getTexture(
			"./media/terrain-texture.jpg"));
	terrain->setMaterialTexture(1, m_driver->getTexture(
			"./media/detailmap3.jpg"));
	terrain->setMaterialType(video::EMT_DETAIL_MAP);
	terrain->scaleTexture(1.0f, 20.0f);

	scene::CDynamicMeshBuffer* buffer = new scene::CDynamicMeshBuffer(
			video::EVT_2TCOORDS, video::EIT_16BIT);
	terrain->getMeshBufferForLOD(*buffer, 0);
	video::S3DVertex2TCoords* data =
			(video::S3DVertex2TCoords*) buffer->getVertexBuffer().getData();

	buffer->drop();

	// create triangle selector for the terrain
	m_terrainSelector = m_smgr->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(m_terrainSelector);

	// create collision response animator and attach it to the camera
	if (GRAVITY_ON) {
		scene::ISceneNodeAnimator
				* anim = m_smgr->createCollisionResponseAnimator(
						m_terrainSelector, m_camera, core::vector3df(60, 100,
								60), core::vector3df(0, -20.f, 0),
						core::vector3df(0, 40, 0));
		m_camera->addAnimator(anim);
		anim->drop();
	}
}

void SceneManager::loadSky() {
	m_driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	scene::ISceneNode* skybox = m_smgr->addSkyBoxSceneNode(
			m_driver->getTexture("./media/irrlicht2_up.jpg"),
			m_driver->getTexture("./media/irrlicht2_dn.jpg"),
			m_driver->getTexture("./media/irrlicht2_lf.jpg"),
			m_driver->getTexture("./media/irrlicht2_rt.jpg"),
			m_driver->getTexture("./media/irrlicht2_ft.jpg"),
			m_driver->getTexture("./media/irrlicht2_bk.jpg"));
	m_driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);
}
