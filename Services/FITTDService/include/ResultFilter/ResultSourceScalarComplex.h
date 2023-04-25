#pragma once
#include "ResultFilter/ResultSource.h"
#include "Simulation/DegreesOfFreedom3DLoopCPU.h"
#include "Ports/Port.h"

template <class T>
class ResultSourceScalarComplex :public ResultSource
{

public:
	ResultSourceScalarComplex(ExecutionBarrier * executionBarrier, IGridIteratorVolume * volumeIter, Port<T> & port, const T (Port<T>::*GetterCurrentValue)() const);
	virtual ResultContainer * GenerateResultContainer(int currentTimeStep) override;

private:
	Port<T> & _port;
	const T(Port<T>::*GetterCurrentValue)() const = nullptr;
};
#include "ResultFilter/ResultSourceScalarComplex.hpp"