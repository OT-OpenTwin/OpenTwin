#pragma once
#include "OTServiceFoundation/UiComponent.h"
#include "OTCore/DefensiveProgramming.h"
#include <chrono>

//! @brief RAII wrapper for the processbar, including  an API to trigger an update with set frequency.
class __declspec(dllexport) ProgressUpdater
{
	friend class FixtureProgressUpdater;
public:
	ProgressUpdater(ot::components::UiComponent* _uiComponent, const std::string& _progressbarMessage, bool _continuous = false);
	ProgressUpdater(const ProgressUpdater& _other) = delete;
	ProgressUpdater(ProgressUpdater&& _other) = delete;
	ProgressUpdater operator=(ProgressUpdater& _other) = delete;
	ProgressUpdater operator=(ProgressUpdater&& _other) = delete;
	~ProgressUpdater();

	//! @brief This would trigger an update _triggerFrequency steps, additionally to the time trigger
	void setTriggerFrequency(uint64_t  _triggerFrequency);
	
	//! @brief Mandatory!
	void setTotalNumberOfSteps(uint64_t  _totalNumberofSteps);
	void setTotalNumberOfUpdates(uint32_t  _numberOfUpdates, uint64_t _totalNumberofSteps);
	
	//! @brief default is set to 2 sec
	void setTimeTrigger(std::chrono::seconds _timeTrigger);
	
	//! @brief requires the total number of steps. Triggers when the time trigger says so, or the current step is a multiple of the triggerfrequency
	void triggerUpdate(int32_t _currentStep);

	uint64_t getTriggerFrequency() const { return m_triggerFrequency; }
	uint64_t getTotalNumberOfSteps() const { return m_totalNumberOfSteps; }
private:
	ProgressUpdater() {};
	inline bool invariant() { return m_uiComponent != nullptr; }
	std::chrono::steady_clock::time_point m_priorTimestamp;
	ot::components::UiComponent* m_uiComponent = nullptr;
	uint64_t m_triggerFrequency = 0;
	uint64_t m_totalNumberOfSteps = 0;
	int32_t m_priorStep = 0;
	std::chrono::seconds m_timeTrigger = std::chrono::seconds(2);

};
