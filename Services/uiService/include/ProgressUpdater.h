#pragma once

// std header
#include <chrono>

class AppBase;

class ProgressUpdater {
public:
	ProgressUpdater(std::string& _message, uint64_t _totalNbSteps);
	~ProgressUpdater();

	void triggerUpdate(uint64_t _currentStep);

private:
	AppBase* m_app;
	double m_totalNbSteps;
	std::chrono::steady_clock::time_point m_priorTimestamp;
	std::chrono::seconds m_timeTrigger;

};
