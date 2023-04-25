#define _USE_MATH_DEFINES
#include <math.h> 

#include "ResultFilter/ResultFilterDFT.h"

void ResultFilterDFT::ExecuteFilter(int currentTimestep, double * input)
{
	_volumeIterator->Reset();
	CoefficientsUpdate(currentTimestep);

	int i = 0;
	input[i] *= _realCoefficient;
	input[i + _offsetToComplex] *= _imagCoefficient;

	while (_volumeIterator->HasNext())
	{
		i++;
		_volumeIterator->GetNextIndex();

		input[i] *= _realCoefficient;
		input[i + _offsetToComplex] = input[i] *_imagCoefficient;
	}
}

ResultFilterDFT::ResultFilterDFT(int executionFrequency, int totalTimeSteps, double observedFrequency)
	: ResultFilter(executionFrequency, totalTimeSteps), _frequency(observedFrequency)
{};


void ResultFilterDFT::CoefficientsUpdate(int currentTimestep)
{
	if (_currentTimestep < currentTimestep)
	{
		_currentTimestep = currentTimestep;

		_realCoefficient = cos(2 * M_PI * _frequency * currentTimestep / (_lastTimeStep + 1));
		_imagCoefficient = sin(2 * M_PI * _frequency * currentTimestep / (_lastTimeStep + 1));
	}
}