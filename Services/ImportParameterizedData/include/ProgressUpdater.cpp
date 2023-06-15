#include "ProgressUpdater.h"

ProgressUpdater::ProgressUpdater(ot::components::UiComponent* uiComponent, const std::string& progressbarMessage)
	: _uiComponent(uiComponent) 
{
	_uiComponent->setProgressInformation(progressbarMessage, false);
	_uiComponent->setProgress(0);
}

ProgressUpdater::~ProgressUpdater()
{
	if (_uiComponent != nullptr)
	{
		_uiComponent->closeProgressInformation();
	}
}

void ProgressUpdater::SetTriggerFrequency(uint64_t triggerFrequency)
{
	_triggerFrequency = triggerFrequency;
}

void ProgressUpdater::SetTotalNumberOfSteps(uint64_t totalNumberofSteps)
{
	_totalNumberOfSteps = totalNumberofSteps;
}

void ProgressUpdater::SetTotalNumberOfUpdates(uint32_t numberOfUpdates, uint64_t totalNumberofSteps)
{
	_totalNumberOfSteps = totalNumberofSteps;
	_triggerFrequency = _totalNumberOfSteps / numberOfUpdates;
}

void ProgressUpdater::TriggerUpdate(int32_t currentStep)
{
	if (currentStep != 0 && currentStep != _totalNumberOfSteps && currentStep % _triggerFrequency == 0)
	{
		_uiComponent->setProgress( static_cast<int>(static_cast<float>(currentStep) / _totalNumberOfSteps * 100));
	}
}
