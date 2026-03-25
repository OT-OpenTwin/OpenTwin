// @otlicense

// OpenTwin header
#include "OTWidgets/Diagram/SequenceDiaSceneBuilder.h"

ot::SequenceDiaSceneBuilder::SequenceDiaSceneBuilder(GraphicsView* _graphicsView)
	: m_view(_graphicsView)
{
}

ot::SequenceDiaSceneBuilder::~SequenceDiaSceneBuilder()
{
}

void ot::SequenceDiaSceneBuilder::build()
{
	if (!m_needsBuild)
	{
		return;
	}



	m_needsBuild = false;
}

void ot::SequenceDiaSceneBuilder::addFunction(const SequenceDiaFunctionCfg& _functionCfg)
{

}
