#include <stdafx.h>
#include "TextVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "Notifier.h"

void TextVisualiser::visualise()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, m_entityID, doc.GetAllocator());

	getNotifier()->messageModelService(doc.toJson());
}
