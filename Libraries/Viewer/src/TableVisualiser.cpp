#include "stdafx.h"
#include "TableVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "Notifier.h"

void TableVisualiser::visualise()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TABLE_Setup, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, m_entityID, doc.GetAllocator());

	getNotifier()->messageModelService(doc.toJson());
}
