#pragma once
#include "OutputPipeline.h"
class OutputPipelineRAII
{
public:
	OutputPipelineRAII(OutputPipeline::RedirectionMode _targetMode)
	{
		m_previousMode = OutputPipeline::instance().getRedirectOutputMode();
		OutputPipeline::instance().setRedirectOutputMode(_targetMode);
	}

	~OutputPipelineRAII()
	{
		OutputPipeline::instance().setRedirectOutputMode(m_previousMode);
	}
private:
	OutputPipeline::RedirectionMode m_previousMode;
};

