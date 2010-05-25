#ifndef LOCATIONTRACKER_H_
#define LOCATIONTRACKER_H_

#include <vector>
using namespace std;

class LocationTracker {
public:
	LocationTracker();
	virtual ~LocationTracker();

	void insertNewValue(int val, unsigned long timestamp);

	unsigned int getTimeIntervalOfThrow();
	float getDistanceIntervalOfThrow(); // percentage of max radius
	bool newThrowReady();

	void calibrateFront(int val);
	void calibrateBack(int val);
	void reset();

private:
	int m_front;
	int m_back;

	bool m_newThrowReady;
	bool m_newThrowReady_query;
	bool m_backwardsReady;

	int m_prevLastStableValue;
	int m_lastStableValue;
	unsigned int m_timestampPrevLastStableValue;
	unsigned int m_timestampLastStableValue;
	vector<int> m_cachedValues;

	const static int thresholdValue = 15;
	const static int nbStableValuesBeforeStable = 10;

	int getAverageCachedValue();
};

#endif /* LOCATIONTRACKER_H_ */
