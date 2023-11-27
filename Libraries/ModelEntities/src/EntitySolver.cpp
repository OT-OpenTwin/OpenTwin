
#include "EntitySolver.h"
#include "Types.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

EntitySolver::EntitySolver(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityContainer(ID, parent, obs, ms, factory, owner)
{
}

EntitySolver::~EntitySolver()
{
}

void EntitySolver::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "SolverVisible";
		treeIcons.hiddenIcon = "SolverHidden";

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

bool EntitySolver::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	getProperties().forceResetUpdateForAllProperties();

	return false; // No property grid update necessary
}
