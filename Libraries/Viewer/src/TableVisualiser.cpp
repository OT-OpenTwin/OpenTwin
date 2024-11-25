#include "stdafx.h"
#include "TableVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "Notifier.h"
#include "SceneNodeBase.h"

TableVisualiser::TableVisualiser(SceneNodeBase* _sceneNode) 
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewTable)
{

}

void TableVisualiser::visualise()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, OT_ACTION_CMD_UI_TABLE_Setup, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());

	getNotifier()->messageModelService(doc.toJson());
}
