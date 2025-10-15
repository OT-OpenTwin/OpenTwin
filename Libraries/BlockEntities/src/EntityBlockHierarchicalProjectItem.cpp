//! @file EntityBlockHierarchicalProjectItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "PropertyHelper.h"
#include "EntityProperties.h"
#include "EntityBlockHierarchicalProjectItem.h"

static EntityFactoryRegistrar<EntityBlockHierarchicalProjectItem> registrar(EntityBlockHierarchicalProjectItem::className());

EntityBlockHierarchicalProjectItem::EntityBlockHierarchicalProjectItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
	: EntityBlock(_ID, _parent, _obs, _ms, _owner)
{
	OldTreeIcon icon;
	icon.visibleIcon = "ProjectTemplates/DefaultIcon";
	icon.hiddenIcon = "ProjectTemplates/DefaultIcon";
	setNavigationTreeIcon(icon);

	setBlockTitle("Hierarchical Project Item");

	resetModified();
}

ot::GraphicsItemCfg* EntityBlockHierarchicalProjectItem::createBlockCfg() {
	return nullptr;
}

bool EntityBlockHierarchicalProjectItem::updateFromProperties() {
	if (getProperties().anyPropertyNeedsUpdate()) {
		createBlockItem();
	}

	getProperties().forceResetUpdateForAllProperties();
	return true;
}

void EntityBlockHierarchicalProjectItem::createProperties() {
	EntityPropertiesBase* prop = EntityPropertiesString::createProperty("Project Data", "ProjectName", "", "", getProperties());
	prop->setVisible(false);
	prop->setReadOnly(true);

	prop = EntityPropertiesString::createProperty("Project Data", "ProjectType", "", "", getProperties());
	prop->setVisible(false);
	prop->setReadOnly(true);

	prop = EntityPropertiesString::createProperty("Project Data", "CollectionName", "", "", getProperties());
	prop->setVisible(false);
	prop->setReadOnly(true);
}

void EntityBlockHierarchicalProjectItem::setProjectInformation(const ot::ProjectInformation& _info) {
	PropertyHelper::setStringPropertyValue(_info.getProjectName(), this, "ProjectName");
	PropertyHelper::setStringPropertyValue(_info.getProjectType(), this, "ProjectType");
	PropertyHelper::setStringPropertyValue(_info.getCollectionName(), this, "CollectionName");
	setModified();
}

ot::ProjectInformation EntityBlockHierarchicalProjectItem::getProjectInformation() const {
	ot::ProjectInformation info;
	
	info.setProjectName(PropertyHelper::getStringPropertyValue(this, "ProjectName"));
	info.setProjectType(PropertyHelper::getStringPropertyValue(this, "ProjectType"));
	info.setCollectionName(PropertyHelper::getStringPropertyValue(this, "CollectionName"));

	return info;
}
