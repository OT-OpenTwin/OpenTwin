#pragma once
#include "OTServiceFoundation/UiComponent.h"
//! @brief RAII wrapper for the processbar, including  an API to trigger an update with set frequency.
class __declspec(dllexport) ProgressUpdater
{
public:
	ProgressUpdater(ot::components::UiComponent* uiComponent, const std::string& progressbarMessage);
	ProgressUpdater(ProgressUpdater& other) = delete;
	ProgressUpdater(ProgressUpdater&& other) = delete;
	ProgressUpdater operator=(ProgressUpdater& other) = delete;
	ProgressUpdater operator=(ProgressUpdater&& other) = delete;
	~ProgressUpdater();

	void SetTriggerFrequency(uint64_t  triggerFrequency);
	void SetTotalNumberOfSteps(uint64_t  totalNumberofSteps);
	void SetTotalNumberOfUpdates(uint32_t  numberOfUpdates, uint64_t totalNumberofSteps);
	void TriggerUpdate(int32_t currentStep);
private:
	ot::components::UiComponent* _uiComponent = nullptr;
	uint64_t _triggerFrequency = 0;
	uint64_t _totalNumberOfSteps = 0;
};
