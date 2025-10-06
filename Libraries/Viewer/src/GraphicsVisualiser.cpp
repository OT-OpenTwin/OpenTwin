#include "stdafx.h"
#include "GraphicsVisualiser.h"

#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "OTGui/VisualisationCfg.h"

GraphicsVisualiser::GraphicsVisualiser(SceneNodeBase* _sceneNode)
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics)
{
}

bool GraphicsVisualiser::requestVisualization(const VisualiserState& _state)
{
	if (_state.m_selected)
	{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
			

		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());
			
		ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state);
		visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor);

		ot::JsonObject visualisationCfgJSon;
		visualisationCfg.addToJsonObject(visualisationCfgJSon, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_Visualisation_Config, visualisationCfgJSon, doc.GetAllocator());

		FrontendAPI::instance()->messageModelService(doc.toJson());
		return true;
	}
	return false;
}

void GraphicsVisualiser::showVisualisation(const VisualiserState& _state)
{
}

void GraphicsVisualiser::hideVisualisation(const VisualiserState& _state)
{
}

void GraphicsVisualiser::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("Type", ot::JsonString("GraphicsVisualiser", _allocator), _allocator);
	Visualiser::getDebugInformation(_object, _allocator);
}
