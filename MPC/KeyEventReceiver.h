#ifndef KEYEVENTRECEIVER_H_
#define KEYEVENTRECEIVER_H_

#include <irrlicht.h>
using namespace irr;

class KeyEventReceiver: public IEventReceiver {
public:

	KeyEventReceiver() {
		m_stopApp = false;
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
			default:
				break;
			}
		}

		return false;
	}

private:
	bool m_stopApp;
};

#endif
