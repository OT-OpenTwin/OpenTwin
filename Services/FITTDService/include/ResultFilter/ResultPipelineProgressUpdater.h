#pragma once
#include "ResultFilter/ResultFilter.h"
/* Does not work so far. Either it gets the uiComponent (breaks encapsulation) or a callBackfunction to memberfunction of the calling class.*/
#include "OTServiceFoundation/UiComponent.h"
//#include <boost/function.hpp>
class ResultFilterProgressUpdater : public ResultFilter
{
	ot::components::UiComponent * _uiComponent;
	virtual void ExecuteFilter(int currentTimestep, double * input) override;

public:
	ResultFilterProgressUpdater(int executionFrequency, int totalSteps, ot::components::UiComponent * uiComponent);
	~ResultFilterProgressUpdater();

	virtual void SetVolumeIterator(IGridIteratorVolume * volumenFilter) override;
};