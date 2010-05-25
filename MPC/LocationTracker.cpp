#include "LocationTracker.h"
#include <math.h>
#include <cmath>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

LocationTracker::LocationTracker() {
	reset();
}

LocationTracker::~LocationTracker() {
}

void LocationTracker::insertNewValue(int val, unsigned long timestamp) {
	if (m_newThrowReady)
		return; // reset for next throw
	else {
		bool stable = false;

		if (m_cachedValues.size() == 0)
			stable = true;
		else {
			int avg = getAverageCachedValue();
			if (abs(1.0 * avg - val) <= thresholdValue)
				stable = true;
		}

		if (!stable) {
			m_cachedValues.clear();
			m_cachedValues.push_back(val);
			return;
		} else {
			m_cachedValues.push_back(val);
			if (m_cachedValues.size() == nbStableValuesBeforeStable
					&& !m_newThrowReady) {
				if (!m_backwardsReady) {
					cout << "BACK STABLE\n";
					m_backwardsReady = true;
					m_prevLastStableValue = getAverageCachedValue();
					m_timestampPrevLastStableValue = timestamp;
				} else {
					cout << "FRONT STABLE -> THROW READY\n";
					m_newThrowReady = true;
					m_lastStableValue = getAverageCachedValue();
					m_timestampLastStableValue = timestamp;
				}
			}
		}
	}
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
	if (!m_newThrowReady_query) {
		if (m_newThrowReady)
			m_newThrowReady_query = true;
		return m_newThrowReady;

	} else
		return false;
}

void LocationTracker::calibrateFront(int val) {
	m_front = val;
}

void LocationTracker::calibrateBack(int val) {
	m_back = val;
}

void LocationTracker::reset() {
	m_newThrowReady = false;
	m_backwardsReady = false;
	m_timestampLastStableValue = 0;
	m_timestampPrevLastStableValue = 0;
	m_cachedValues.clear();
	m_newThrowReady_query = false;
}

int LocationTracker::getAverageCachedValue() {
	int result = 0;
	for (unsigned int i = 0; i < m_cachedValues.size(); ++i)
		result += m_cachedValues[i];

	if (m_cachedValues.size() > 0) {
		return (int) (1.0 * result / m_cachedValues.size());
	} else
		return 0;
}
