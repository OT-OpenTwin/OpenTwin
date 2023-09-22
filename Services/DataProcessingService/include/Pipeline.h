#pragma once
#include "PipelineSource.h"
#include "PipelineSink.h"
#include "PipelineFilter.h"
#include "PipelineItem.h"

class Pipeline
{

public:
	Pipeline();

	void RunPipeline();
	void SetSource(PipelineSource* sourceBlock) { _sourceBlock = sourceBlock; };
	void AddFilter(PipelineFilter&& newFilter) { _filter.push_back(newFilter); };
	void AddSink(PipelineSink* newSink) { _sinks.push_back(newSink); };

private:
	PipelineSource* _sourceBlock;
	std::list<PipelineFilter> _filter;
	std::list<PipelineSink*> _sinks;
};
