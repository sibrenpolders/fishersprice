#ifndef LOCATIONTRACKER_H_
#define LOCATIONTRACKER_H_

class LocationTracker {
public:
	LocationTracker();
	virtual ~LocationTracker();

	void insertNewValues(int* vals, unsigned long timestamp);

	// Niet zeker... mss gewoon altijd rechtdoor gooien, en mbv een encoder het
	// beeld op 't scherm laten veranderen.
	int* getPositionInPlane(int* vals); // two-dimensional in screenCoords

	unsigned int getTimeIntervalOfThrow();
	float getDistanceIntervalOfThrow(); // percentage of max radius
	bool newThrowReady();

	void calibrateUpperLeft(int* vals);
	void calibrateUpperRight(int* vals);
	void calibrateLowerLeft(int* vals);
	void calibrateBackwards(int* vals);
	void setScreenDimensions(int width, int height);
	void reset();

private:
	int m_upperLeft[3];
	int m_lowerLeft[3];
	int m_upperRight[3];
	int m_back[3];

	int m_screenWidth, m_screenHeight;

	bool m_newThrowReady; // set back on false when pos is read out
	int m_prevLastStableValuesBackwards[3];
	int m_lastStableValuesBackwards[3];
	int m_lastInsertedValues[3];
	bool m_lastStableWasEndOfThrow;
	bool m_lastInsertedWasStable;
	unsigned int m_timestampPrevLastStableValues;
	unsigned int m_timestampLastStableValues;

	const static int thresholdValue = 15;
};

#endif /* LOCATIONTRACKER_H_ */
