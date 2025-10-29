// @otlicense

#pragma once
#include "ResultSource.h"
#include "Simulation/DegreesOfFreedom3DLoopCPU.h"
#include "Ports/SourceTarget.h"

template <class T>
class ResultSourceVector3DComplex : public ResultSource
{
public:
	ResultSourceVector3DComplex(ExecutionBarrier * executionBarrier, IGridIteratorVolume * volumeIter, DegreesOfFreedom3DLoopCPU<T> &doF, sourceType targetType);
	virtual ResultContainer * GenerateResultContainer(int currentTimeStep) override;
	
private:
	DegreesOfFreedom3DLoopCPU<T> & _doF;
	T*(DegreesOfFreedom3DLoopCPU<T>::*GetterX)(index_t index) const = nullptr;
	T*(DegreesOfFreedom3DLoopCPU<T>::*GetterY)(index_t index) const = nullptr;
	T*(DegreesOfFreedom3DLoopCPU<T>::*GetterZ)(index_t index) const = nullptr;

};

#include "ResultFilter/ResultSourceVector3DComplex.hpp"