// @otlicense
// File: ResultFilterSink.cpp
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

#include "ResultFilter/ResultFilterSink.h"


ResultFilterSink::ResultFilterSink(int executionFrequency, int totalTimeSteps, std::vector<ResultFilter*> finalFilter = std::vector<ResultFilter*>()) 
	:ResultFilter(executionFrequency, totalTimeSteps), _finalFilter(finalFilter)
{
	_numberOfExecutions = static_cast<int>(std::ceil(totalTimeSteps / static_cast<float>(executionFrequency)));
}

void ResultFilterSink::SetVolumeIterator(IGridIteratorVolume * volumenFilter)
{
	_volumenIterator = volumenFilter;
}
