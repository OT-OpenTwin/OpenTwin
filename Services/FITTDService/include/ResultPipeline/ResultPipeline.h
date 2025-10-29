// @otlicense

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