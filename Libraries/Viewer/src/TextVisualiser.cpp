#include <stdafx.h>
#include "TextVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "OTCore/LogDispatcher.h"


TextVisualiser::TextVisualiser(SceneNodeBase* _sceneNode)
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewText)
{
}

bool TextVisualiser::requestVisualization(const VisualiserState& _state)
{
	if (_state.m_selectionData.getSelectionOrigin() == ot::SelectionOrigin::User)
	{
		if(_state.m_singleSelection)
		{
			if(_state.m_selected)
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, doc.GetAllocator());

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
		else
		{
			OT_LOG_I("Visualisation of a multiselection of texts is turned off for performance reasons.");
		}
	}
	return false;
}

void TextVisualiser::showVisualisation(const VisualiserState& _state) {

}

void TextVisualiser::hideVisualisation(const VisualiserState& _state) {

}

void TextVisualiser::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Type", ot::JsonString("TextVisualiser", _allocator), _allocator);
	Visualiser::getDebugInformation(_object, _allocator);
}