//#include "Pipeline.h"
//
//Pipeline::Pipeline()
//{}
//
//void Pipeline::RunPipeline()
//{
//	PipelineItem::genericDataBlock parameter;
//	auto sourceData = _sourceBlock->CreateData(parameter);
//	for(PipelineFilter& filter : _filter)
//	{
//		sourceData = filter.FilterData(sourceData);
//	}
//	for (PipelineSink* sink : _sinks)
//	{
//		sink->ProcessData(sourceData);
//	}
//}
