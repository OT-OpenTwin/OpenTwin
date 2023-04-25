#include "ResultFilter/ResultPipelineProgressUpdater.h"


ResultFilterProgressUpdater::ResultFilterProgressUpdater(int executionFrequency, int totalSteps, ot::components::UiComponent * uiComponent)
	:ResultFilter(executionFrequency, totalSteps), _uiComponent(uiComponent)
{
	uiComponent->setProgressInformation("Simulation progress", true);
}

ResultFilterProgressUpdater::~ResultFilterProgressUpdater()
{
	_uiComponent->closeProgressInformation();
}

void ResultFilterProgressUpdater::SetVolumeIterator(IGridIteratorVolume * volumenFilter)
{
}


void ResultFilterProgressUpdater::ExecuteFilter(int currentTimestep, double * input)
{
	int percentage = static_cast<int>(100 * currentTimestep/ static_cast<float>(this->_lastTimeStep));
	_uiComponent->setProgress(percentage);
}
