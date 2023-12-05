#include "EntityResearchMetadata.h"

#include "OTCommunication/ActionTypes.h"

EntityResearchMetadata::EntityResearchMetadata(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	: EntityWithDynamicFields(ID, parent, mdl, ms, factory, owner)
{
}

bool EntityResearchMetadata::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

void EntityResearchMetadata::addVisualizationNodes()
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = "RMD";
		treeIcons.hiddenIcon = "RMD";


		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);

		getObserver()->sendMessageToViewer(doc);
	}

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}
