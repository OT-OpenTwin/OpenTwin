// @otlicense
// File: ResultPipelineProgressUpdater.h
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
#include "ResultFilter/ResultFilter.h"
/* Does not work so far. Either it gets the uiComponent (breaks encapsulation) or a callBackfunction to memberfunction of the calling class.*/
#include "OTServiceFoundation/UiComponent.h"
//#include <boost/function.hpp>
class ResultFilterProgressUpdater : public ResultFilter
{
	ot::components::UiComponent * _uiComponent;
	virtual void ExecuteFilter(int currentTimestep, double * input) override;

public:
	ResultFilterProgressUpdater(int executionFrequency, int totalSteps, ot::components::UiComponent * uiComponent);
	~ResultFilterProgressUpdater();

	virtual void SetVolumeIterator(IGridIteratorVolume * volumenFilter) override;
};