// @otlicense
// File: ResultSourceScalarComplex.hpp
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