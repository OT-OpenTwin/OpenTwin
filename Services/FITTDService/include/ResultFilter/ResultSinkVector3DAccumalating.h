// @otlicense
// File: ResultSinkVector3DAccumalating.h
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
#include "ResultFilter/ResultSink.h"
#include "ResultFilter/ResultContainerVector3DComplex.h"
#include <vector>


class ResultSinkVector3DAccumalating : public ResultSink
{
public:
	ResultSinkVector3DAccumalating(int executionFrequency, int totalTimeSteps, double deltaT)
		:ResultSink(executionFrequency, totalTimeSteps), _deltaT(deltaT) {};

	void SetVolumeIterator(IGridIteratorVolume * volumeIter) override;
	void ConsumeResultContainer(ResultContainer * container) override;

	const double * GetResultX() const { return _resultX.data(); };
	const double * GetResultY() const { return _resultY.data(); };
	const double * GetResultZ() const { return _resultZ.data(); };
	const double * GetTimesteps() const { return _timeSteps.data(); };
	const index_t GetResultContainerSize() const { return _resultX.size(); };

	virtual void PerformPostProcessing() override;

private:
	double _deltaT;
	std::vector<double> _timeSteps;
	std::vector<double> _resultX;
	std::vector<double> _resultY;
	std::vector<double> _resultZ;
};