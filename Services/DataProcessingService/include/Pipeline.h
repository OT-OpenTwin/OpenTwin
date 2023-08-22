#pragma once
#include "PipelineSource.h"
#include "PipelineSink.h"
#include "PipelineFilter.h"
#include "PipelineItem.h"

class Pipeline
{
public:
	Pipeline(PipelineSource sourceBlock);
	void RunPipeline();

private:
	//const PipelineSource _sourceBlock;
	std::list<PipelineFilter> _filter;
	std::list<PipelineSink> _sinks;
};
