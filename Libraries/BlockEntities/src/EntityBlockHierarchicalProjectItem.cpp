//! @file EntityBlockHierarchicalProjectItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "PropertyHelper.h"
#include "EntityProperties.h"
#include "EntityBlockHierarchicalProjectItem.h"

#include "OTGui/GraphicsHierarchicalProjectItemBuilder.h"

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
	ot::ProjectInformation project = this->getProjectInformation();

	ot::GraphicsHierarchicalProjectItemBuilder builder;
	
	builder.setName(this->getName());
	builder.setTitle(this->createBlockHeadline());
	builder.setLeftTitleCornerImagePath("ProjectTemplates/" + project.getProjectType());
	builder.setTitleBackgroundGradientColor(ot::Green);

	return builder.createGraphicsItem();
}

bool EntityBlockHierarchicalProjectItem::updateFromProperties() {
	bool updateGrid = false;

	if (getProperties().anyPropertyNeedsUpdate()) {
		createBlockItem();
	}

	bool useLatestVersionProp = PropertyHelper::getBoolPropertyValue(this, "UseLatestsVersion");
	EntityPropertiesString* customVersionProp = PropertyHelper::getStringProperty(this, "CustomVersion");
	OTAssertNullptr(customVersionProp);

	updateGrid = (useLatestVersionProp == customVersionProp->getVisible());
	customVersionProp->setVisible(!useLatestVersionProp);

	if (customVersionProp->getValue().empty()) {
		customVersionProp->setValue("1");
	}

	getProperties().forceResetUpdateForAllProperties();

	return updateGrid;
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



	prop = EntityPropertiesBoolean::createProperty("Project", "UseLatestsVersion", true, "", getProperties());
	prop = EntityPropertiesString::createProperty("Project", "CustomVersion", "1", "", getProperties());
	prop->setVisible(false);
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

void EntityBlockHierarchicalProjectItem::setUseLatestVersion(bool _flag) {
	PropertyHelper::setBoolPropertyValue(_flag, this, "UseLatestsVersion");
}

bool EntityBlockHierarchicalProjectItem::getUseLatestVersion() const {
	return PropertyHelper::getBoolPropertyValue(this, "UseLatestsVersion");
}

void EntityBlockHierarchicalProjectItem::setCustomVersion(const std::string& _version) {
	PropertyHelper::setStringPropertyValue(_version, this, "CustomVersion");
}

std::string EntityBlockHierarchicalProjectItem::getCustomVersion() const {
	return PropertyHelper::getStringPropertyValue(this, "CustomVersion");
}
