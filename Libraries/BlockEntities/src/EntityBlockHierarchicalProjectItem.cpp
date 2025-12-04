// @otlicense
// File: EntityBlockHierarchicalProjectItem.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "EntityFile.h"
#include "PropertyHelper.h"
#include "EntityFileImage.h"
#include "EntityProperties.h"
#include "EntityBlockHierarchicalProjectItem.h"

#include "OTGui/GraphicsHierarchicalProjectItemBuilder.h"

static EntityFactoryRegistrar<EntityBlockHierarchicalProjectItem> registrar(EntityBlockHierarchicalProjectItem::className());

EntityBlockHierarchicalProjectItem::EntityBlockHierarchicalProjectItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_ID, _parent, _obs, _ms), m_previewUID(ot::invalidUID), m_previewVersion(ot::invalidUID), m_previewData(nullptr),
	m_previewFormat(ot::ImageFileFormat::PNG)
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
	builder.setTitleBackgroundGradientColor(ot::Blue);

	// Optional settings
	if (!getUseLatestVersion()) {
		builder.setProjectVersion(getCustomVersion());
	}
	if (m_previewData != nullptr) {
		builder.setPreviewImageData(m_previewData->getData(), m_previewFormat);
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
	EntityPropertiesBase* prop = EntityPropertiesBoolean::createProperty("Project", "Use current version", true, "", getProperties());
	prop->setToolTip("If enabled, the last active version will be used (same as opening the project regulary).");

	prop = EntityPropertiesString::createProperty("Project", "Custom version", "1", "", getProperties());
	prop->setToolTip("Specify a custom version to be used when opening the project.");
	prop->setVisible(false);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data accessors

void EntityBlockHierarchicalProjectItem::setPreviewFile(ot::UID _entityID, ot::UID _entityVersion, ot::ImageFileFormat _format) {
	m_previewUID = _entityID;
	m_previewVersion = _entityVersion;
	m_previewFormat = _format;
	m_previewData.reset();
	m_previewData = nullptr;

	setModified();
}

void EntityBlockHierarchicalProjectItem::removePreviewFile() {
	m_previewUID = ot::invalidUID;
	m_previewVersion = ot::invalidUID;
	m_previewData.reset();
	m_previewData = nullptr;

	setModified();
}

std::shared_ptr<EntityBinaryData> EntityBlockHierarchicalProjectItem::getPreviewFileData() {
	ensurePreviewLoaded();
	return m_previewData;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property accessors

void EntityBlockHierarchicalProjectItem::setProjectInformation(const ot::ProjectInformation& _info) {
	m_projectName = _info.getProjectName();
	m_projectType = _info.getProjectType();
	m_collectionName = _info.getCollectionName();

	OldTreeIcon icon;
	icon.visibleIcon = "ProjectTemplates/" + _info.getProjectType();
	icon.hiddenIcon = "ProjectTemplates/" + _info.getProjectType();
	setNavigationTreeIcon(icon);

	setModified();
}

ot::ProjectInformation EntityBlockHierarchicalProjectItem::getProjectInformation() const {
	ot::ProjectInformation info;
	
	info.setProjectName(m_projectName);
	info.setProjectType(m_projectType);
	info.setCollectionName(m_collectionName);

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
		bsoncxx::builder::basic::kvp("ProjectName", m_projectName),
		bsoncxx::builder::basic::kvp("ProjectType", m_projectType),
		bsoncxx::builder::basic::kvp("CollectionName", m_collectionName),
		bsoncxx::builder::basic::kvp("PreviewImageID", static_cast<int64_t>(m_previewUID)),
		bsoncxx::builder::basic::kvp("PreviewImageVersion", static_cast<int64_t>(m_previewVersion)),
		bsoncxx::builder::basic::kvp("PreviewImageType", ot::toString(m_previewFormat))
	);
}

void EntityBlockHierarchicalProjectItem::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);

	m_projectName = _docView["ProjectName"].get_string().value.data();
	m_projectType = _docView["ProjectType"].get_string().value.data();
	m_collectionName = _docView["CollectionName"].get_string().value.data();
	m_previewUID = static_cast<ot::UID>(_docView["PreviewImageID"].get_int64());
	m_previewVersion = static_cast<ot::UID>(_docView["PreviewImageVersion"].get_int64());
	m_previewFormat = ot::stringToImageFileFormat(_docView["PreviewImageType"].get_string().value.data());
}

void EntityBlockHierarchicalProjectItem::ensurePreviewLoaded() {
	if (m_previewUID == ot::invalidUID) {
		return;
	}
	if (m_previewData != nullptr) {
		return;
	}

	std::map<ot::UID, EntityBase*> entityMap;
	m_previewData.reset(dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, m_previewUID, m_previewVersion, entityMap)));
}
