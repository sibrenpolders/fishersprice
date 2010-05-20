#ifndef LOCATIONTRACKER_H_
#define LOCATIONTRACKER_H_

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

	bool m_newThrowReady; // set back on false when pos is read out
	int m_prevLastStableValue;
	int m_lastStableValue;
	int m_lastInsertedValue;
	bool m_lastStableWasEndOfThrow;
	bool m_lastInsertedWasStable;
	unsigned int m_timestampPrevLastStableValue;
	unsigned int m_timestampLastStableValue;

	const static int thresholdValue = 15;
};

#endif /* LOCATIONTRACKER_H_ */
