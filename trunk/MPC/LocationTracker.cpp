#include "LocationTracker.h"
#include <math.h>
#include <cmath>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
using namespace std;

LocationTracker::LocationTracker() {
	reset();
}

LocationTracker::~LocationTracker() {
}

void LocationTracker::insertNewValue(int val, unsigned long timestamp) {
	// instable values... do nothing
	if (abs(1.0 * m_lastInsertedValue - val) >= thresholdValue) {
		m_newThrowReady = false;
		m_lastInsertedWasStable = false;
	} else {
		if (m_lastStableWasEndOfThrow && !m_lastInsertedWasStable) {
			m_lastStableWasEndOfThrow = false;
			m_timestampPrevLastStableValue = timestamp;
			m_prevLastStableValue = val;
		} else if (!m_lastInsertedWasStable) {
			m_lastStableWasEndOfThrow = true;
			m_newThrowReady = true;
			m_timestampLastStableValue = timestamp;
			m_lastStableValue = val;
		}

		m_lastInsertedWasStable = true;
	}

	m_lastInsertedValue = val;
}

unsigned int LocationTracker::getTimeIntervalOfThrow() {
	return m_timestampLastStableValue - m_timestampPrevLastStableValue;
}

float LocationTracker::getDistanceIntervalOfThrow() {
	int maxDiff = abs(1.0 * m_back - m_front);
	int diff = abs(1.0 * m_prevLastStableValue - m_lastStableValue);

	float result = (1.0 * diff) / maxDiff;
	return result;
}

bool LocationTracker::newThrowReady() {
	bool result = m_newThrowReady;
	m_newThrowReady = false;

	return result;
}

void LocationTracker::calibrateFront(int val) {
	m_front = val;
}

void LocationTracker::calibrateBack(int val) {
	m_back = val;
}

void LocationTracker::reset() {
	m_lastStableWasEndOfThrow = true;
	m_newThrowReady = false;
	m_timestampLastStableValue = 0;
	m_timestampPrevLastStableValue = 0;
}
