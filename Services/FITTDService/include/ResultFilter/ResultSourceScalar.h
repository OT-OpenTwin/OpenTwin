// @otlicense
// File: ResultSourceScalar.h
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