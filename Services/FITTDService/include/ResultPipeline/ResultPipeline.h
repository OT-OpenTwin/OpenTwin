// @otlicense
// File: ResultPipeline.h
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
#include "Grid/IGridIteratorVolume.h"
#include "ResultFilter/ResultFilter.h"
#include "ResultFilter/ResultSource.h"
#include "ResultFilter/ResultSink.h"

#include <vector>
#include <string>

class ResultPipeline
{
public:
	ResultPipeline(ResultSource * source, ResultSink * sink);
	~ResultPipeline() 
	{
		for (auto filter : _resultFilters)
		{
			delete filter;
		}
		delete _sink;
		delete _source;
	};
	
	void AddResultFilter(ResultFilter * filter);
	void Execute(int currentTimestep);
	ResultSink * GetSink() { return _sink; }

	void SetResultName(std::string resultName) { _resultName = resultName; }
	void SetResultLegendLabel(std::string label) { _resultLegendLabel = label; }
	void SetResultTitle(std::string title) { _resultTitle = title; }
	void SetLabelYAxis(std::string label) { _labelYAxis = label; }
	void SetUnitYAxis(std::string label) { _unitYAxis = label; }
	void SetLabelXAxis(std::string label) { _labelXAxis = label; }
	void SetUnitXAxis(std::string label) { _unitXAxis = label; }


	const std::string GetResultName() const { return _resultName; }
	const std::string GetResultLegendLabel() const { return _resultLegendLabel; }
	const std::string GetResultTitle() const { return _resultTitle; }
	
	const std::string GetLabelYAxis() const { return _labelYAxis; }
	const std::string GetUnitYAxis() const { return _unitYAxis; }
	const std::string GetLabelXAxis() const { return _labelXAxis; }
	const std::string GetUnitXAxis() const { return _unitXAxis; }

private:
	std::string _resultName;
	std::string _resultLegendLabel;
	std::string _resultTitle;
	
	std::string _labelYAxis;
	std::string _labelXAxis;
	std::string _unitYAxis;
	std::string _unitXAxis;

	std::vector<ResultFilter*> _resultFilters;
	ResultSink * _sink;
	ResultSource * _source;
};