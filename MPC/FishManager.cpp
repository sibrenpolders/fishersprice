#include "FishManager.h"

FishManager::FishManager(IrrlichtDevice *device, IVideoDriver* driver,
		ISceneManager* smgr) {
	this->m_device = device;
	this->m_driver = driver;
	this->m_smgr = smgr;
	m_nextID = 0;
}

FishManager::~FishManager() {
}

void FishManager::update(unsigned long millisecs) {
	for (unsigned int i = 0; i < m_fishes.size(); ++i) {
		FishInfo* info = m_fishes[i];
		if (!info->isHooked) {
			// Check if the fish must return
			if (info->currMilliSecsPassed + millisecs >= info->duration) {
				if (info->fromStartToStop) {
					info->fromStartToStop = false;
					info->direction = info->start;
					info->currMilliSecsPassed = 0;
					info->node->setRotation(vector3df(0.0, 180.0, 0.0));
				} else {
					info->fromStartToStop = true;
					info->direction = info->stop;
					info->currMilliSecsPassed = 0;
					info->node->setRotation(vector3df(0.0, 0.0, 0.0));
				}
			}

			// Update the position
			info->currMilliSecsPassed += millisecs;
			float ratio = ((float) info->currMilliSecsPassed) / info->duration;

			if (info->fromStartToStop) {
				vector3df curr = info->start + ratio * (info->stop
						- info->start);
				info->currPos = curr;
				info->node->setPosition(curr);
			} else {
				vector3df curr = info->stop + ratio
						* (info->start - info->stop);
				info->currPos = curr;
				info->node->setPosition(curr);
			}
		}
	}
}

void FishManager::addFish(TYPE_FISH fish, vector3df startPos, vector3df endPos,
		int duration) {
	std::string meshName = getMeshName(fish);
	std::string textureName = getTextureName(fish);
	core::vector3df scale = getScale(fish);

	IAnimatedMesh* mesh = m_smgr->getMesh(meshName.c_str());
	if (!mesh)
		exit(0);
	IAnimatedMeshSceneNode* node = m_smgr->addAnimatedMeshSceneNode(mesh);
	if (node) {
		node->setMaterialFlag(EMF_LIGHTING, true);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture(0, m_driver->getTexture(textureName.c_str()));
		node->setPosition(startPos);
		node->setScale(scale);

		FishInfo* newInfo = new FishInfo();
		newInfo->currPos = startPos;
		newInfo->start = startPos;
		newInfo->stop = endPos;
		newInfo->duration = duration;
		newInfo->node = node;
		newInfo->type = fish;
		newInfo->currMilliSecsPassed = 0;
		newInfo->fromStartToStop = true;
		newInfo->ID = m_nextID++;
		newInfo->isHooked = false;
		newInfo->direction = endPos;

		m_fishes.push_back(newInfo);
	}
}

std::string FishManager::getMeshName(TYPE_FISH fish) {
	switch (fish) {
	case CLOWN:
		return "./media/mesh_clownfish.obj";
		break;
	case PERCH:
		return "./media/mesh_perch.obj";
		break;
	case SHARK:
		return "./media/shark.obj";
		break;
	case MORAY:
		return "./media/moray_eel.obj";
		break;
	default:
		return "./media/mesh_clownfish.obj";
		break;
	}
	return "./media/mesh_clownfish.obj";
}

std::string FishManager::getTextureName(TYPE_FISH fish) {
	switch (fish) {
	case CLOWN:
		return "./media/clownfish.bmp";
		break;
	case PERCH:
		return "./media/perch_texture.bmp";
		break;
	case SHARK:
		return "./media/sharktexture.bmp";
		break;
	case MORAY:
		return "./media/morayeel_texture.bmp";
		break;
	default:
		return "./media/clownfish.bmp";
		break;
	}
	return "./media/clownfish.bmp";
}

core::vector3df FishManager::getScale(TYPE_FISH fish) {
	switch (fish) {
	case CLOWN:
		return core::vector3df(4.f, 4.f, 4.f);
		break;
	case PERCH:
		return core::vector3df(5.f, 5.f, 5.f);
		break;
	case SHARK:
		return core::vector3df(15.f, 15.f, 15.f);
		break;
	case MORAY:
		return core::vector3df(8.f, 8.f, 8.f);
		break;
	default:
		return core::vector3df(4.f, 4.f, 4.f);
		break;
	}
	return core::vector3df(4.f, 4.f, 4.f);

}
