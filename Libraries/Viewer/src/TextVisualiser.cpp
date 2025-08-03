#include <stdafx.h>
#include "TextVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "OTCore/Logger.h"


TextVisualiser::TextVisualiser(SceneNodeBase* _sceneNode)
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewText)
{
}

bool TextVisualiser::requestVisualization(const VisualiserState& _state)
{
	if (!m_viewIsOpen && _state.m_selectionOrigin == ot::SelectionOrigin::User)
	{
		if(_state.m_singleSelection)
		{
			if(_state.m_selected)
			{
				OT_LOG_T("Reqesting text");

				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, doc.GetAllocator());

				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _state.m_setFocus, doc.GetAllocator());
				
				ot::UIDList visualizingEntities;
				visualizingEntities.push_back(this->getSceneNode()->getModelEntityID());
				doc.AddMember(OT_ACTION_PARAM_VisualizingEntities, ot::JsonArray(visualizingEntities, doc.GetAllocator()), doc.GetAllocator());

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
