// @otlicense
// File: ResultSourceScalar.hpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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