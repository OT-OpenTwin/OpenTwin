#include "OTServiceFoundation/ProgressUpdater.h"
#include <cmath>
#include <thread>

ProgressUpdater::ProgressUpdater(ot::components::UiComponent* _uiComponent, const std::string& _progressbarMessage)
	: m_uiComponent(_uiComponent)
{
	INVARIANT;
	m_uiComponent->setProgressInformation(_progressbarMessage, false);
	m_uiComponent->setProgress(0);	
}

ProgressUpdater::~ProgressUpdater()
{
	if (m_uiComponent != nullptr)
	{
		m_uiComponent->closeProgressInformation();
	}
}

void ProgressUpdater::setTriggerFrequency(uint64_t _triggerFrequency)
{
	m_triggerFrequency = _triggerFrequency;
	POST(m_triggerFrequency > 0);
}

void ProgressUpdater::setTotalNumberOfSteps(uint64_t _totalNumberofSteps)
{
	m_totalNumberOfSteps = _totalNumberofSteps;
	POST(m_totalNumberOfSteps > 0);
}

void ProgressUpdater::setTotalNumberOfUpdates(uint32_t _numberOfUpdates, uint64_t _totalNumberofSteps)
{
	m_totalNumberOfSteps = _totalNumberofSteps;
	m_triggerFrequency = static_cast<uint64_t>(std::floor(m_totalNumberOfSteps / _numberOfUpdates));
	if (m_triggerFrequency == 0)
	{
		m_triggerFrequency = 1;
	}
	POST(m_totalNumberOfSteps > 0 && m_triggerFrequency > 0);
}

void ProgressUpdater::setTimeTrigger(std::chrono::seconds _timeTrigger)
{
	m_timeTrigger = _timeTrigger;
}

void ProgressUpdater::triggerUpdate(int32_t _currentStep)
{
	PRE(m_totalNumberOfSteps > 0);
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_priorTimestamp);
	m_priorTimestamp = currentTime;

	bool triggerUpdate = _currentStep != 0; //First step is skipped
	triggerUpdate &= _currentStep != m_totalNumberOfSteps; //Last step is skipped
	triggerUpdate &= _currentStep != m_priorStep; // If we still are in the same step, we don't want to trigger

	
	bool triggerUpdateByStep = false;
	if (m_triggerFrequency != 0)
	{
		triggerUpdateByStep = _currentStep% m_triggerFrequency == 0; //If a trigger frequency is set, we want to trigger an update every x steps
	}

	if (triggerUpdate && (duration >= m_timeTrigger || triggerUpdateByStep))
	{
		const uint32_t percentage = static_cast<uint32_t>(std::lround(static_cast<float>(_currentStep) / m_totalNumberOfSteps * 100));
		m_uiComponent->setProgress(percentage);
	}

	m_priorStep = _currentStep;
}
