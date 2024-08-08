#include "OTServiceFoundation/ProgressUpdater.h"

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

void ProgressUpdater::triggerUpdate(int32_t _currentStep)
{
	PRE(m_totalNumberOfSteps > 0 && m_triggerFrequency > 0);

	const bool isFirstStep = _currentStep == 0;
	const bool isLastStep = _currentStep == m_totalNumberOfSteps;
	const bool currentStepShouldTrigger = _currentStep% m_triggerFrequency == 0;

	if (!isFirstStep && !isLastStep && currentStepShouldTrigger)
	{
		const uint32_t percentage =  static_cast<uint32_t>(_currentStep / m_totalNumberOfSteps * 100);
		m_uiComponent->setProgress(percentage);
	}
}
