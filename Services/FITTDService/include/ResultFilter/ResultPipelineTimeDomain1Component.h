// @otlicense

#pragma once

#include "ResultPipeline/ResultPipeline.h"
#include "Simulation/DegreesOfFreedom3DLoopCPU.h"
#include "SystemDependencies/SystemDependentDefines.h"
#include <vector>

template <class T>
class ResultPipelineTimeDomain1Component : public ResultPipeline
{
private:
	DegreesOfFreedom3DLoopCPU<T> &_doF;
	ResultVolumeFilter<T> * _filter;
	std::vector<T> _result;
	std::vector<T> _timeSteps;
	
	T*(DegreesOfFreedom3DLoopCPU<T>::*GetterOfDoF)(index_t index) const;
public:
	ResultPipelineTimeDomain1Component(int executionFrequency, int totalTimeSteps, DegreesOfFreedom3DLoopCPU<T> & doF, T*(DegreesOfFreedom3DLoopCPU<T>::*GetterDoF)(index_t index) const, ResultVolumeFilter<T> * filter)
		: ResultPipeline(executionFrequency, totalTimeSteps),  _doF(doF),  _filter(filter) , GetterOfDoF(GetterDoF)
	{
		_result.reserve(_lastTimeStep + 1);
		_timeSteps.reserve(_lastTimeStep + 1);

	};
	~ResultPipelineTimeDomain1Component()
	{
		delete _filter;
		_filter = nullptr;
	}
	const std::vector<T> & GetResultVector() const { return _result; };
	const std::vector<T> & GetTimeSteps() const { return _timeSteps; };

	virtual void Execute(int currentTimestep) override
	{
		_timeSteps.push_back(static_cast<T>(currentTimestep));
		auto temp = (_doF.*GetterOfDoF)(0);
		const T* result = _filter->FilterVolume(temp);
		_result.push_back(result[0]);
	}
};