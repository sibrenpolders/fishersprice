#include "GUIManager.h"

GUIManager::GUIManager(IrrlichtDevice *device) {
	m_device = device;
	init();
}

GUIManager::~GUIManager() {
}

void GUIManager::init() {
	IGUIEnvironment* env = m_device->getGUIEnvironment();
	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("./media/fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);
	skin->setFont(env->getBuiltInFont(), EGDF_TOOLTIP);

	IGUIElement* root = env->getRootGUIElement();
	IGUIElement* e = root->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
	if (e)
		e->remove();

	// create the toolbox window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(SCREEN_WIDTH - 280, 0,
			SCREEN_WIDTH, 500), false, L"", 0, GUI_ID_DIALOG_ROOT_WINDOW);
	IGUITabControl* tab = env->addTabControl(core::rect<s32>(2,20,278,493), wnd, true, true);
	IGUITab* t1 = tab->addTab(L"Output");
	env->addStaticText(L"Received Arduino message:", core::rect<s32>(10,20,280,45), false, true, t1);
	env->addStaticText(L"N/A", core::rect<s32>(20,45,280,95), true, true, t1, GUI_ID_RECEIVED_ARDUINO);

	env->addStaticText(L"Parsed sensor values:", core::rect<s32>(10,125,280,150), false, true, t1);
	env->addStaticText(L"Accelerometer = ", core::rect<s32>(20,150,115,175), false, true, t1);
	env->addStaticText(L"N/A", core::rect<s32>(115,150,160,175), true, true, t1, GUI_ID_ACCEL);

	env->addStaticText(L"Encoder = ", core::rect<s32>(20,175,115,200), false, true, t1);
	env->addStaticText(L"N/A", core::rect<s32>(115,175,160,200), true, true, t1, GUI_ID_ENCODER);

	env->addStaticText(L"Potentiometer = ", core::rect<s32>(20,200,115,225), false, true, t1);
	env->addStaticText(L"N/A", core::rect<s32>(115,200,160,225), true, true, t1, GUI_ID_POT);

	env->addStaticText(L"Pushbutton = ", core::rect<s32>(20,225,115,250), false, true, t1);
	env->addStaticText(L"N/A", core::rect<s32>(115,225,160,250), true, true, t1, GUI_ID_PUSH);

	env->addStaticText(L"Switch = ", core::rect<s32>(20,250,115,275), false, true, t1);
	env->addStaticText(L"N/A", core::rect<s32>(115,250,160,275), true, true, t1, GUI_ID_SWITCH);

	env->addStaticText(L"Game message:", core::rect<s32>(10,305,280,330), false, true, t1);
	env->addStaticText(L"N/A", core::rect<s32>(20,330,280,400), true, true, t1, GUI_ID_GAME_MESSAGE);
}

void GUIManager::setText(GUI_ID e, std::string txt) {
	IGUIElement
			* toolboxWnd =
					m_device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(
							GUI_ID_DIALOG_ROOT_WINDOW, true);
	if (!toolboxWnd)
		return;

	toolboxWnd->getElementFromId(e, true)->setText(
			core::stringw(txt.c_str()).c_str());
}
