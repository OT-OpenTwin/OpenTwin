//! @file EntityBlockHierarchicalProjectItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "EntityFile.h"
#include "PropertyHelper.h"
#include "EntityFileImage.h"
#include "EntityProperties.h"
#include "EntityBlockHierarchicalProjectItem.h"

#include "OTGui/GraphicsHierarchicalProjectItemBuilder.h"

static EntityFactoryRegistrar<EntityBlockHierarchicalProjectItem> registrar(EntityBlockHierarchicalProjectItem::className());

EntityBlockHierarchicalProjectItem::EntityBlockHierarchicalProjectItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
	: EntityBlock(_ID, _parent, _obs, _ms, _owner), m_previewUID(ot::invalidUID), m_previewVersion(ot::invalidUID), m_preview(nullptr)
{
	OldTreeIcon icon;
	icon.visibleIcon = "ProjectTemplates/DefaultIcon";
	icon.hiddenIcon = "ProjectTemplates/DefaultIcon";
	setNavigationTreeIcon(icon);

	setBlockTitle("Hierarchical Project Item");

	resetModified();
}

ot::GraphicsItemCfg* EntityBlockHierarchicalProjectItem::createBlockCfg() {
	ensurePreviewLoaded();

	ot::ProjectInformation project = this->getProjectInformation();
	ot::GraphicsHierarchicalProjectItemBuilder builder;
	
	// Mandatory settings
	builder.setName(this->getName());
	builder.setTitle(this->createBlockHeadline());
	builder.setProjectType(project.getProjectType());
	builder.setLeftTitleCornerImagePath("ProjectTemplates/" + project.getProjectType());
	builder.setTitleBackgroundGradientColor(ot::Green);

	// Optional settings
	if (!getUseLatestVersion()) {
		builder.setProjectVersion(getCustomVersion());
	}
	if (m_preview != nullptr) {
		builder.setPreviewImageData(m_preview->getImage(), m_preview->getImageFormat());
	}

	// Create the item
	return builder.createGraphicsItem();
}

bool EntityBlockHierarchicalProjectItem::updateFromProperties() {
	bool updateGrid = false;

	bool useLatestVersionProp = PropertyHelper::getBoolPropertyValue(this, "Use current version");
	EntityPropertiesString* customVersionProp = PropertyHelper::getStringProperty(this, "Custom version");
	OTAssertNullptr(customVersionProp);

	updateGrid = (useLatestVersionProp == customVersionProp->getVisible());
	customVersionProp->setVisible(!useLatestVersionProp);

	if (customVersionProp->getValue().empty()) {
		customVersionProp->setValue("1");
	}

	getProperties().forceResetUpdateForAllProperties();

	createBlockItem();

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

	prop = EntityPropertiesBoolean::createProperty("Project", "Use current version", true, "", getProperties());
	prop->setToolTip("If enabled, the last active version will be used (same as opening the project regulary).");

	prop = EntityPropertiesString::createProperty("Project", "Custom version", "1", "", getProperties());
	prop->setToolTip("Specify a custom version to be used when opening the project.");
	prop->setVisible(false);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data accessors

void EntityBlockHierarchicalProjectItem::setPreviewFile(ot::UID _entityID, ot::UID _entityVersion) {
	m_previewUID = _entityID;
	m_previewVersion = _entityVersion;
	m_preview.reset();
	m_preview = nullptr;

	setModified();
}

void EntityBlockHierarchicalProjectItem::removePreviewFile() {
	m_previewUID = ot::invalidUID;
	m_previewVersion = ot::invalidUID;
	m_preview.reset();
	m_preview = nullptr;

	setModified();
}

std::shared_ptr<EntityFileImage> EntityBlockHierarchicalProjectItem::getPreviewFile() {
	ensurePreviewLoaded();
	return m_preview;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property accessors

void EntityBlockHierarchicalProjectItem::setProjectInformation(const ot::ProjectInformation& _info) {
	PropertyHelper::setStringPropertyValue(_info.getProjectName(), this, "ProjectName");
	PropertyHelper::setStringPropertyValue(_info.getProjectType(), this, "ProjectType");
	PropertyHelper::setStringPropertyValue(_info.getCollectionName(), this, "CollectionName");

	OldTreeIcon icon;
	icon.visibleIcon = "ProjectTemplates/" + _info.getProjectType();
	icon.hiddenIcon = "ProjectTemplates/" + _info.getProjectType();
	setNavigationTreeIcon(icon);

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
	PropertyHelper::setBoolPropertyValue(_flag, this, "Use current version");
}

bool EntityBlockHierarchicalProjectItem::getUseLatestVersion() const {
	return PropertyHelper::getBoolPropertyValue(this, "Use current version");
}

void EntityBlockHierarchicalProjectItem::setCustomVersion(const std::string& _version) {
	PropertyHelper::setStringPropertyValue(_version, this, "Custom version");
}

std::string EntityBlockHierarchicalProjectItem::getCustomVersion() const {
	return PropertyHelper::getStringPropertyValue(this, "Custom version");
}

void EntityBlockHierarchicalProjectItem::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityBlock::addStorageData(_storage);

	_storage.append(
		bsoncxx::builder::basic::kvp("PreviewImageID", static_cast<int64_t>(m_previewUID)),
		bsoncxx::builder::basic::kvp("PreviewImageVersion", static_cast<int64_t>(m_previewVersion))
	);
}

void EntityBlockHierarchicalProjectItem::readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);

	m_previewUID = static_cast<ot::UID>(_docView["PreviewImageID"].get_int64());
	m_previewVersion = static_cast<ot::UID>(_docView["PreviewImageVersion"].get_int64());
}

void EntityBlockHierarchicalProjectItem::ensurePreviewLoaded() {
	if (m_previewUID == ot::invalidUID) {
		return;
	}
	if (m_preview != nullptr) {
		return;
	}

	std::map<ot::UID, EntityBase*> entityMap;
	m_preview.reset(dynamic_cast<EntityFileImage*>(readEntityFromEntityIDAndVersion(this, m_previewUID, m_previewVersion, entityMap)));
}
