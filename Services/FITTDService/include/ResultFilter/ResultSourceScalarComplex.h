// @otlicense
// File: ResultSourceScalarComplex.h
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