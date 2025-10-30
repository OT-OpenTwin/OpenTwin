// @otlicense
// File: ResultSourceVector3DComplex.hpp
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

#include "ResultFilter/ResultSourceVector3DComplex.h"
#include "ResultContainerVector3DComplex.h"

template<class T>
inline ResultSourceVector3DComplex<T>::ResultSourceVector3DComplex(ExecutionBarrier * executionBarrier, IGridIteratorVolume * volumeIter, DegreesOfFreedom3DLoopCPU<T> &doF, sourceType targetType)
	: ResultSource(executionBarrier, volumeIter), _doF(doF)
{
	if (targetType == e)
	{
		GetterX = &DegreesOfFreedom3DLoopCPU<T>::GetElectricVoltageXComponent;
		GetterY = &DegreesOfFreedom3DLoopCPU<T>::GetElectricVoltageYComponent;
		GetterZ = &DegreesOfFreedom3DLoopCPU<T>::GetElectricVoltageZComponent;
	}
	else
	{
		GetterX = &DegreesOfFreedom3DLoopCPU<T>::GetMagneticVoltageXComponent;
		GetterY = &DegreesOfFreedom3DLoopCPU<T>::GetMagneticVoltageYComponent;
		GetterZ = &DegreesOfFreedom3DLoopCPU<T>::GetMagneticVoltageZComponent;
	}
	
}

template<class T>
inline ResultContainer * ResultSourceVector3DComplex<T>::GenerateResultContainer(int timestep)
{
	_volumeIterator->Reset();
	index_t size = _volumeIterator->GetVolumenSize();
	double * resultCompX = new double[size*2];
	double * resultCompY = new double[size*2];
	double * resultCompZ = new double[size*2];

	int i = 0;
	index_t index = _volumeIterator->GetCurrentIndex();
	resultCompX[i] = *(_doF.*GetterX)(index);
	resultCompY[i] = *(_doF.*GetterY)(index);
	resultCompZ[i] = *(_doF.*GetterZ)(index);

	while (_volumeIterator->HasNext())
	{
		index = _volumeIterator->GetNextIndex();
		i++;
		resultCompX[i] = *(_doF.*GetterX)(index);
		resultCompY[i] = *(_doF.*GetterY)(index);
		resultCompZ[i] = *(_doF.*GetterZ)(index);
		resultCompX[i+size] = 0;
		resultCompY[i+size] = 0;
		resultCompZ[i+size] = 0;
	}
	auto result = new ResultContainerVector3DComplex(timestep, size,resultCompX, resultCompY, resultCompZ);
	return result;
}

