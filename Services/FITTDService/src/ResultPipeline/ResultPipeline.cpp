// @otlicense
// File: ResultPipeline.cpp
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

#include "ResultPipeline/ResultPipeline.h"

ResultPipeline::ResultPipeline(ResultSource * source, ResultSink * sink)
	: _source(source) , _sink(sink)
{
	sink->SetVolumeIterator(_source->GetSubvolumeIterator());
};


void ResultPipeline::AddResultFilter(ResultFilter* filter)
{
	
	filter->SetVolumeIterator(_source->GetSubvolumeIterator());
	_resultFilters.push_back(filter);
}


void ResultPipeline::Execute(int currentTimestep)
{
	if (_source->ShallBeExecuted(currentTimestep))
	{
		auto resultContainer =  _source->GenerateResultContainer(currentTimestep);
		for (auto filter : _resultFilters)
		{
			resultContainer->ApplyFilter(filter);
		}
		_sink->ConsumeResultContainer(resultContainer);
	}
}
