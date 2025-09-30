#include "EntitySolverDataProcessing.h"
#include "IVisualisationGraphicsView.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/VisualisationTypes.h"

void EntitySolverDataProcessing::addVisualizationNodes(void)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "SolverVisible";
	treeIcons.hiddenIcon = "SolverHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	ot::VisualisationTypes visTypes;
	visTypes.addGraphicsViewVisualisation();
	visTypes.addToJsonObject(doc, doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);

	EntityBase::addVisualizationNodes();
}

ot::GraphicsNewEditorPackage* EntitySolverDataProcessing::getGraphicsEditorPackage()
{
	ot::GraphicsNewEditorPackage* editor = new ot::GraphicsNewEditorPackage(getName(), getName());
	return editor;
}

bool EntitySolverDataProcessing::visualiseGraphicsView()
{
	return true;
}
