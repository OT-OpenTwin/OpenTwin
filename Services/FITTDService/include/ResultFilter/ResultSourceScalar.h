// @otlicense

#pragma once
#include "ResultFilter/ResultSource.h"
#include "Simulation/DegreesOfFreedom3DLoopCPU.h"

template <class T>
class ResultSourceScalar :public ResultSource
{

public:
	ResultSourceScalar(ExecutionBarrier* executionBarrier, IGridIteratorVolume * volumeIter, DegreesOfFreedom3DLoopCPU<T> & doF, T*(DegreesOfFreedom3DLoopCPU<T>::*GetterDoF)(index_t index) const);
	virtual ResultContainer * GenerateResultContainer(int currentTimeStep) override;

private:
	DegreesOfFreedom3DLoopCPU<T> & _doF;
	T*(DegreesOfFreedom3DLoopCPU<T>::*GetterOfDoF)(index_t index) const = nullptr;
};

#include "ResultFilter/ResultSourceScalar.hpp"