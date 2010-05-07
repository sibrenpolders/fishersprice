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

// X,Y,Z
// Y = UP, X = SIDEWAYS, Z = DEPTH
void LocationTracker::insertNewValues(int* vals, unsigned long timestamp) {
	// instable values... do nothing
	if (abs(1.0 * m_lastInsertedValues[2] - vals[2]) >= thresholdValue) {
		m_newThrowReady = false;
		m_lastInsertedWasStable = false;
	} else {
		m_lastInsertedWasStable = true;

		if (m_lastStableWasEndOfThrow && !m_lastInsertedWasStable) {
			m_lastStableWasEndOfThrow = false;
			m_timestampPrevLastStableValues = timestamp;
			memcpy(m_prevLastStableValuesBackwards, vals, sizeof(int) * 3);
		} else if (!m_lastInsertedWasStable) {
			m_lastStableWasEndOfThrow = true;
			m_newThrowReady = false;
			m_timestampLastStableValues = timestamp;
			memcpy(m_lastStableValuesBackwards, vals, sizeof(int) * 3);
		}
	}

	memcpy(m_lastInsertedValues, vals, sizeof(int) * 3);
}

// column, row
// 0,0 is bottomleft
int* LocationTracker::getPositionInPlane(int* vals) {
	int* result = (int*) malloc(sizeof(int) * 2);

	int maxDiffX = abs(1.0 * m_upperRight[0] - m_upperLeft[0]);
	int maxDiffY = abs(1.0 * m_upperLeft[1] - m_lowerLeft[1]);

	int diffX = abs(1.0 * m_lastStableValuesBackwards[0] - m_upperLeft[0]);
	int diffY = abs(1.0 * m_lastStableValuesBackwards[1] - m_lowerLeft[0]);

	float ratioX = (1.0 * diffX) / maxDiffX;
	float ratioY = (1.0 * diffY) / maxDiffY;

	int X = (int) (m_screenWidth * ratioX);
	int Y = (int) (m_screenHeight * ratioY);

	result[0] = X;
	result[1] = Y;

	return result;
}

unsigned int LocationTracker::getTimeIntervalOfThrow() {
	return m_timestampLastStableValues - m_timestampPrevLastStableValues;
}

float LocationTracker::getDistanceIntervalOfThrow() {
	int maxDiff = abs(1.0 * m_back[2] - m_lowerLeft[2]);
	int diff = abs(1.0 * m_prevLastStableValuesBackwards[2]
			- m_lastStableValuesBackwards[2]);

	float result = (1.0 * diff) / maxDiff;
	return result;
}

bool LocationTracker::newThrowReady() {
	bool result = m_newThrowReady;
	m_newThrowReady = false;

	return result;
}

void LocationTracker::calibrateUpperLeft(int* vals) {
	memcpy(m_upperLeft, vals, sizeof(int) * 3);
}

void LocationTracker::calibrateUpperRight(int* vals) {
	memcpy(m_upperRight, vals, sizeof(int) * 3);
}

void LocationTracker::calibrateLowerLeft(int* vals) {
	memcpy(m_lowerLeft, vals, sizeof(int) * 3);
}

void LocationTracker::calibrateBackwards(int* vals) {
	memcpy(m_back, vals, sizeof(int) * 3);
}

void LocationTracker::setScreenDimensions(int width, int height) {
	m_screenHeight = height;
	m_screenWidth = width;
}

void LocationTracker::reset() {
	m_lastStableWasEndOfThrow = true;
	m_newThrowReady = false;
	m_timestampLastStableValues = 0;
	m_timestampPrevLastStableValues = 0;
}
