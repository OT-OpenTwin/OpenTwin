#include "stdafx.h"
#include "GraphicsVisualiser.h"

#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"

GraphicsVisualiser::GraphicsVisualiser(SceneNodeBase* _sceneNode)
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics)
{
}

bool GraphicsVisualiser::requestVisualization(const VisualiserState& _state)
{
	if (_state.m_singleSelection)
	{
		if (_state.m_selected)
		{
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, doc.GetAllocator());

			doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _state.m_setFocus, doc.GetAllocator());

			ot::UIDList visualizingEntities;
			visualizingEntities.push_back(this->getSceneNode()->getModelEntityID());
			doc.AddMember(OT_ACTION_PARAM_VisualizingEntities, ot::JsonArray(visualizingEntities, doc.GetAllocator()), doc.GetAllocator());

			if (_state.m_selectionData.getKeyboardModifiers() & (Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier)) {
				doc.AddMember(OT_ACTION_PARAM_SuppressViewHandling, true, doc.GetAllocator());
			}
			else {
				doc.AddMember(OT_ACTION_PARAM_SuppressViewHandling, false, doc.GetAllocator());
			}

			FrontendAPI::instance()->messageModelService(doc.toJson());
			return true;
		}
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
