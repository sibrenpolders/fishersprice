#ifndef GUIMANAGER_H_
#define GUIMANAGER_H_

#include <irrlicht.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;
using namespace gui;

#define SCREEN_WIDTH 1280//1024
#define SCREEN_HEIGTH 800//768
typedef enum GUI_ID {
	GUI_ID_DIALOG_ROOT_WINDOW = 0x10000,
	GUI_ID_RECEIVED_ARDUINO,
	GUI_ID_ACCEL,
	GUI_ID_ENCODER,
	GUI_ID_POT,
	GUI_ID_PUSH,
	GUI_ID_SWITCH,
	GUI_ID_GAME_MESSAGE,
} GUI_ID;

class GUIManager {
public:
	GUIManager(IrrlichtDevice *device);
	virtual ~GUIManager();
	void setText(GUI_ID e, std::string txt);

private:
	IrrlichtDevice* m_device;
	void init();
};

#endif /* GUIMANAGER_H_ */
