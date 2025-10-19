//! @file EntityBlockHierarchicalDocumentItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "EntityFile.h"
#include "PropertyHelper.h"
#include "EntityProperties.h"
#include "EntityBlockHierarchicalDocumentItem.h"

#include "OTGui/GraphicsHierarchicalProjectItemBuilder.h"

static EntityFactoryRegistrar<EntityBlockHierarchicalDocumentItem> registrar(EntityBlockHierarchicalDocumentItem::className());

EntityBlockHierarchicalDocumentItem::EntityBlockHierarchicalDocumentItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
	: EntityBlock(_ID, _parent, _obs, _ms, _owner), m_documentUID(ot::invalidUID), m_documentEntity(nullptr) {
	OldTreeIcon icon;
	icon.visibleIcon = "Hierarchical/Document";
	icon.hiddenIcon = "Hierarchical/Document";
	//setNavigationTreeIcon(icon);

	setBlockTitle("Hierarchical Document Item");

	resetModified();
}

ot::GraphicsItemCfg* EntityBlockHierarchicalDocumentItem::createBlockCfg() {
	ot::GraphicsHierarchicalProjectItemBuilder builder;

	// Mandatory settings
	builder.setName(this->getName());
	builder.setTitle(this->createBlockHeadline());
	builder.setLeftTitleCornerImagePath("Hierarchical/Document");
	builder.setTitleBackgroundGradientColor(ot::Red);
	builder.setPreviewImagePath("Hierarchical/DocumentBackground");

	// Create the item
	return builder.createGraphicsItem();
}

bool EntityBlockHierarchicalDocumentItem::updateFromProperties() {
	return EntityBlock::updateFromProperties();
}

void EntityBlockHierarchicalDocumentItem::createProperties() {
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data accessors

void EntityBlockHierarchicalDocumentItem::setDocument(ot::UID _entityID) {
	m_documentUID = _entityID;
	m_documentEntity.reset();
	m_documentEntity = nullptr;

	setModified();
}

std::shared_ptr<EntityBase> EntityBlockHierarchicalDocumentItem::getDocument() {
	ensureDocumentLoaded();
	return m_documentEntity;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property accessors

void EntityBlockHierarchicalDocumentItem::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityBlock::addStorageData(_storage);

	_storage.append(
		bsoncxx::builder::basic::kvp("DocumentID", static_cast<int64_t>(m_documentUID))
	);
}

void EntityBlockHierarchicalDocumentItem::readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);

	m_documentUID = static_cast<ot::UID>(_docView["DocumentID"].get_int64());
}

void EntityBlockHierarchicalDocumentItem::ensureDocumentLoaded() {
	if (m_documentUID == ot::invalidUID) {
		return;
	}
	if (m_documentEntity != nullptr) {
		return;
	}

	std::map<ot::UID, EntityBase*> entityMap;
	m_documentEntity.reset(readEntityFromEntityID(this, m_documentUID, entityMap));
}
