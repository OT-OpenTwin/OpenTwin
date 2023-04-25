#pragma once
#include "ResultFilter/ResultSourceScalar.h"
#include "Grid/GridIteratorSubvolume.h"
#include "ResultFilter/ResultContainerScalar.h"

template<class T>
inline ResultSourceScalar<T>::ResultSourceScalar(ExecutionBarrier * executionBarrier, IGridIteratorVolume * volumeIter, DegreesOfFreedom3DLoopCPU<T>& doF, T *(DegreesOfFreedom3DLoopCPU<T>::* GetterDoF)(index_t index) const)
	: ResultSource(executionBarrier,volumeIter), _doF(doF), GetterOfDoF(GetterDoF)
{}

template<class T>
inline ResultContainer * ResultSourceScalar<T>::GenerateResultContainer(int currentTimeStep)
{
	_volumeIterator->Reset();
	index_t size = _volumeIterator->GetVolumenSize();
	double * result = new double[size];

	int i = 0;
	index_t index = _volumeIterator->GetCurrentIndex();
	result[i] = *(_doF.*GetterOfDoF)(index);

	while (_volumeIterator->HasNext())
	{
		index = _volumeIterator->GetNextIndex();
		i++;
		result[i] = *(_doF.*GetterOfDoF)(index);
	}
	auto resultContainer = new ResultContainerScalar(result,size, currentTimeStep);
	return resultContainer;
}