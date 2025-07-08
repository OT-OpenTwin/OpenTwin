#pragma once
#include "AppBase.h"
#include <chrono>

class ProgressUpdater
{
public:
	ProgressUpdater(std::string& _message, uint64_t _totalNbSteps)
		: m_totalNbSteps(static_cast<double>(_totalNbSteps))
	{
		m_app = AppBase::instance();
		assert(m_app != nullptr);
		m_app->setProgressBarVisibility(_message.c_str(), true, false);
		m_priorTimestamp = std::chrono::steady_clock::now();
		m_app->setProgressBarValue(0);
	}

	~ProgressUpdater()
	{
		m_app->setProgressBarVisibility("", false, false);
	}

	void triggerUpdate(uint64_t _currentStep)
	{
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_priorTimestamp);
		if (m_totalNbSteps > 0 && (duration >= m_timeTrigger) || _currentStep == m_totalNbSteps)
		{
			uint32_t percentage = static_cast<uint32_t>((_currentStep / m_totalNbSteps) * 100 + 0.5);
			m_app->setProgressBarValue(percentage);
			m_priorTimestamp = currentTime;
		}
	}

private:
	AppBase* m_app = nullptr;
	double m_totalNbSteps;
	std::chrono::steady_clock::time_point m_priorTimestamp;
	std::chrono::seconds m_timeTrigger = std::chrono::seconds(2);

};
