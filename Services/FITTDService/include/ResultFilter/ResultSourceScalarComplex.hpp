// @otlicense

#pragma once
#include "ResultFilter/ResultSourceScalarComplex.h"
#include "ResultFilter/ResultContainerScalarComplex.h"

template<class T>
inline ResultSourceScalarComplex<T>::ResultSourceScalarComplex(ExecutionBarrier* executionBarrier, IGridIteratorVolume * volumeIter, Port<T> & port, const T (Port<T>::*GetterCurrentValue)() const)
	: ResultSource(executionBarrier, volumeIter), _port(port), GetterCurrentValue(GetterCurrentValue)
{
}

template<class T>
inline ResultContainer * ResultSourceScalarComplex<T>::GenerateResultContainer(int currentTimeStep)
{
	double * result = new double[2];
	result[0] = (_port.*GetterCurrentValue)();
	result[1] = 0;

	auto resultContainer = new ResultContainerScalarComplex(result,2, currentTimeStep);
	return resultContainer;
}