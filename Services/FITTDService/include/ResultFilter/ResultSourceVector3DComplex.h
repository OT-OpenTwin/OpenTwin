// @otlicense
// File: ResultSourceVector3DComplex.h
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