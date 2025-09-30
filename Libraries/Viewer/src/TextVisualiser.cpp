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
	if (_state.m_selectionData.getSelectionOrigin() == ot::SelectionOrigin::User)
	{
		if(_state.m_singleSelection)
		{
			if(_state.m_selected)
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());

				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());
				
				ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state);
				visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_TEXTEDITOR_Setup);

				ot::JsonObject visualisationCfgJSon;
				visualisationCfg.addToJsonObject(visualisationCfgJSon, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_Visualisation_Config, visualisationCfgJSon, doc.GetAllocator());

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