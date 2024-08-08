#pragma once
#include "OTServiceFoundation/UiComponent.h"
#include "OTCore/DefensiveProgramming.h"
//! @brief RAII wrapper for the processbar, including  an API to trigger an update with set frequency.
class __declspec(dllexport) ProgressUpdater
{
	friend class FixtureProgressUpdater;
public:
	ProgressUpdater(ot::components::UiComponent* _uiComponent, const std::string& _progressbarMessage);
	ProgressUpdater(const ProgressUpdater& _other) = delete;
	ProgressUpdater(ProgressUpdater&& _other) = delete;
	ProgressUpdater operator=(ProgressUpdater& _other) = delete;
	ProgressUpdater operator=(ProgressUpdater&& _other) = delete;
	~ProgressUpdater();

	void setTriggerFrequency(uint64_t  _triggerFrequency);
	void setTotalNumberOfSteps(uint64_t  _totalNumberofSteps);
	void setTotalNumberOfUpdates(uint32_t  _numberOfUpdates, uint64_t _totalNumberofSteps);
	void triggerUpdate(int32_t _currentStep);
private:
	ProgressUpdater() {};
	inline bool invariant() { return m_uiComponent != nullptr; }

	ot::components::UiComponent* m_uiComponent = nullptr;
	uint64_t m_triggerFrequency = 0;
	uint64_t m_totalNumberOfSteps = 0;
};
