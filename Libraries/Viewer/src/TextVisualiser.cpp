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

void TextVisualiser::visualise(const VisualiserState& _state)
{
	if(_state.m_singleSelection )
	{
		if(_state.m_selected)
		{
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, doc.GetAllocator());

			doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, _state.m_setFocus, doc.GetAllocator());

			FrontendAPI::instance()->messageModelService(doc.toJson());
		}
	}
	else
	{
		OT_LOG_I("Visualisation of a multiselection of texts is turned off for performance reasons.");
	}
}
