// @otlicense

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
