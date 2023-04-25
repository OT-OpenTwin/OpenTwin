#pragma once
#include <vector>
#include <memory>

#include "SystemDependencies/SystemDependentDefines.h"
#include "ResultFilter/ResultFilter.h"
#include "Grid/GridIteratorVolume.h"
#include "Ports/PortSinusGaussian.h"

class ResultFilterDFT : public ResultFilter
{
public:
	ResultFilterDFT(int executionFrequency, int totalTimeSteps, double observedFrequency);

private:
	int _currentTimestep = -1;
	
	double _realCoefficient;
	double _imagCoefficient;

	double _frequency;

	index_t _offsetToComplex;

	inline void CoefficientsUpdate(int currentTimestep);
	virtual void ExecuteFilter(int currentTimestep, double * input) override;

	virtual void SetVolumeIterator(IGridIteratorVolume * volumenFilter) override
	{
		_volumeIterator = volumenFilter;
		_offsetToComplex = _volumeIterator->GetVolumenSize();
	}

};

