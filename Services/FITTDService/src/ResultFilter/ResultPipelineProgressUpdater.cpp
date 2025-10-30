// @otlicense
// File: ResultPipelineProgressUpdater.cpp
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

#include "ResultFilter/ResultPipelineProgressUpdater.h"


ResultFilterProgressUpdater::ResultFilterProgressUpdater(int executionFrequency, int totalSteps, ot::components::UiComponent * uiComponent)
	:ResultFilter(executionFrequency, totalSteps), _uiComponent(uiComponent)
{
	uiComponent->setProgressInformation("Simulation progress", true);
}

ResultFilterProgressUpdater::~ResultFilterProgressUpdater()
{
	_uiComponent->closeProgressInformation();
}

void ResultFilterProgressUpdater::SetVolumeIterator(IGridIteratorVolume * volumenFilter)
{
}


void ResultFilterProgressUpdater::ExecuteFilter(int currentTimestep, double * input)
{
	int percentage = static_cast<int>(100 * currentTimestep/ static_cast<float>(this->_lastTimeStep));
	_uiComponent->setProgress(percentage);
}
