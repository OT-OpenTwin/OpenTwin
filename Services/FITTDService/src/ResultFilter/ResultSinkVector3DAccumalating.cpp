// @otlicense
// File: ResultSinkVector3DAccumalating.cpp
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

#include "ResultFilter/ResultSinkVector3DAccumalating.h"

void ResultSinkVector3DAccumalating::SetVolumeIterator(IGridIteratorVolume * volumeIter)
{
	ResultSink::SetVolumeIterator(volumeIter);
	_timeSteps.reserve(_nbOfExecutions);
	_resultX.reserve(_nbOfExecutions * _volumeIterator->GetVolumenSize());
	_resultY.reserve(_nbOfExecutions * _volumeIterator->GetVolumenSize());
	_resultZ.reserve(_nbOfExecutions * _volumeIterator->GetVolumenSize());
}

void ResultSinkVector3DAccumalating::ConsumeResultContainer(ResultContainer * container)
{
	auto newContainer = dynamic_cast<ResultContainerVector3DComplex*>(container);
	assert(newContainer != nullptr);

	_timeSteps.push_back(newContainer->GetCurrentTimestep()*_deltaT);
	int i = 0;
	_volumeIterator->Reset();
	_resultX.push_back(newContainer->GetRealComponentX()[i]);
	_resultY.push_back(newContainer->GetRealComponentY()[i]);
	_resultZ.push_back(newContainer->GetRealComponentZ()[i]);

	while (_volumeIterator->HasNext())
	{
		i++;
		_volumeIterator->GetNextIndex();
		_resultX.push_back(newContainer->GetRealComponentX()[i]);
		_resultY.push_back(newContainer->GetRealComponentY()[i]);
		_resultZ.push_back(newContainer->GetRealComponentZ()[i]);
	}
	delete newContainer;
	newContainer = nullptr;
}

void ResultSinkVector3DAccumalating::PerformPostProcessing()
{

}
