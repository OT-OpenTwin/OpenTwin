// @otlicense
// File: ResultPostprocessingComplexNormalization.cpp
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

#include "ResultPostProcessors/ResultPostProcessingComplexNormalization.h"

ResultPostProcessingComplexNormalization::ResultPostProcessingComplexNormalization(ResultSinkScalarComplexSum * normalizer) : _normalizer(normalizer)
{}

void ResultPostProcessingComplexNormalization::PerformProstprocessing(double * result, index_t size)
{
	auto normalizerResult = _normalizer->GetResultContainer();
	double realDivisor = *normalizerResult->GetResult();
	double imagDivisor = *normalizerResult->GetResultImag();
	if (realDivisor == 0.)
	{
		for (index_t i = 0; i < size; i++)
		{
			result[i] = 0;
		}
	}
	else
	{
		for (index_t i = 0; i < size; i++)
		{
			result[i] /= realDivisor;
		}
	}
	if (imagDivisor == 0.)
	{
		for (index_t i = 0; i < size; i++)
		{
			result[i + size] = 0;
		}
	}
	else
	{
		for (index_t i = 0; i < size; i++)
		{
			result[i + size] /= imagDivisor;
		}
	}
	
}
