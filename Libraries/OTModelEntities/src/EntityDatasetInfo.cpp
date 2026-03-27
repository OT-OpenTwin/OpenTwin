// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/PropertyHelper.h"
#include "OTModelEntities/EntityDatasetInfo.h"

static EntityFactoryRegistrar<ot::EntityDatasetInfo> registrar(ot::EntityDatasetInfo::className());

ot::EntityDatasetInfo::EntityDatasetInfo(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms)
	: EntityBase(_ID, _parent, _mdl, _ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/Hint");
	treeItem.setHiddenIcon("Tree/Hint");
	this->setDefaultTreeItem(treeItem);

	ot::VisualisationTypes visTypes = this->getVisualizationTypes();
	//visTypes.addTextVisualisation();
	this->setDefaultVisualizationTypes(visTypes);
}

void ot::EntityDatasetInfo::addVisualizationNodes()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

bool ot::EntityDatasetInfo::updateFromProperties()
{
	assert(getProperties().anyPropertyNeedsUpdate());
	setModified();
	getProperties().forceResetUpdateForAllProperties();

	return false; 
}

void ot::EntityDatasetInfo::createProperties(const std::string& _initialProjectName)
{
	EntityPropertiesProjectList* projectListProp = EntityPropertiesProjectList::createProperty("General", "Project", _initialProjectName, "", getProperties());

	getProperties().forceResetUpdateForAllProperties();
}

void ot::EntityDatasetInfo::setProjectName(const std::string& _projectName)
{
	PropertyHelper::setProjectPropertyValue(_projectName, this, "Project", "General");
}

std::string ot::EntityDatasetInfo::getProjectName() const
{
	return PropertyHelper::getProjectPropertyValue(this, "Project", "General");
}