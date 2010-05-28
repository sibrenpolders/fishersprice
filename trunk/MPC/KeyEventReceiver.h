// author: Sibrand Staessens

#ifndef KEYEVENTRECEIVER_H_
#define KEYEVENTRECEIVER_H_

#include <irrlicht.h>
#include "ActionManager.h"
using namespace irr;

class KeyEventReceiver: public IEventReceiver {
public:

	KeyEventReceiver() {
		m_stopApp = false;
	}

	void setActionManager(ActionManager* aMan) {
		m_actionMan = aMan;
	}

	bool getStopApp() {
		return m_stopApp;
	}

	bool OnEvent(const SEvent& event) {
		// check if user presses the key 'Q'
		if (event.EventType == irr::EET_KEY_INPUT_EVENT
				&& !event.KeyInput.PressedDown) {
			switch (event.KeyInput.Key) {
			case irr::KEY_KEY_Q:
				m_stopApp = true;
				return true;
			case irr::KEY_KEY_Z:
				m_actionMan->goLeft();
				return true;
			case irr::KEY_KEY_X:
				m_actionMan->goRight();
				return true;
			case irr::KEY_KEY_P:
				m_actionMan->reset();
				return true;
			case irr::KEY_KEY_T:
				m_actionMan->toggleSwitch();
				return true;
			case irr::KEY_KEY_R:
				m_actionMan->throwIn();
				return true;
			case irr::KEY_KEY_M:
				m_actionMan->bringInOneUnit();
				return true;
			case irr::KEY_KEY_S:
				m_actionMan->startCalibrating();
				return true;
			default:
				break;
			}
		}

		return false;
	}

private:
	bool m_stopApp;
	ActionManager* m_actionMan;
};

#endif
