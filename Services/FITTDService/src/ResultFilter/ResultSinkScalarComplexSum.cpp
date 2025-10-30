// @otlicense
// File: ResultSinkScalarComplexSum.cpp
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
#include "ResultFilter/ResultSinkScalarComplexSum.h"

ResultSinkScalarComplexSum::~ResultSinkScalarComplexSum()
{
	if (_summedUpResult != nullptr)
	{
		delete _summedUpResult;
		_summedUpResult = nullptr;
	}
}

void ResultSinkScalarComplexSum::ConsumeResultContainer(ResultContainer * container)
{
	ResultContainerScalarComplex * newContainer = dynamic_cast<ResultContainerScalarComplex*>(container);
	assert(newContainer != nullptr);
	if (_summedUpResult == nullptr)
	{
		_summedUpResult = newContainer;
	}
	else
	{
		int i = 0;
		_volumeIterator->Reset();
		while (_volumeIterator->HasNext())
		{
			_summedUpResult->GetResult()[i] += newContainer->GetResult()[i];
			_summedUpResult->GetResultImag()[i] += newContainer->GetResult()[i];

			i++;
			_volumeIterator->GetNextIndex();
		}
		delete newContainer;
		newContainer = nullptr;
	}
}

void ResultSinkScalarComplexSum::PerformPostProcessing()
{
}
