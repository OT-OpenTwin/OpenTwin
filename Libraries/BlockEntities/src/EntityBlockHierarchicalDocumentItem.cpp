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
	: EntityBlock(_ID, _parent, _obs, _ms, _owner), m_documentUID(ot::invalidUID), m_documentVersion(ot::invalidUID), m_documentData(nullptr) {
	OldTreeIcon icon;
	icon.visibleIcon = "Hierarchical/Document";
	icon.hiddenIcon = "Hierarchical/Document";
	setNavigationTreeIcon(icon);

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

void EntityBlockHierarchicalDocumentItem::setDocument(ot::UID _entityID, ot::UID _entityVersion, const std::string& _documentType, const std::string& _documentExtension) {
	m_documentUID = _entityID;
	m_documentVersion = _entityVersion;
	m_documentType = _documentType;
	m_documentExtension = _documentExtension;
	m_documentData.reset();
	m_documentData = nullptr;

	setModified();
}

std::string EntityBlockHierarchicalDocumentItem::getText() {
	ensureDocumentDataLoaded();

	if (m_documentData) {
		return std::string(m_documentData->getData().data(), m_documentData->getData().size());
	}
	else {
		return std::string();
	}
}

void EntityBlockHierarchicalDocumentItem::setText(const std::string& _text) {
	ensureDocumentDataLoaded();
	if (!m_documentData) {
		return;
	}
	std::vector<char> data(_text.begin(), _text.end());
	m_documentData->setData(std::move(data));
	m_documentData->storeToDataBase();
	m_documentVersion = m_documentData->getEntityStorageVersion();

	setModified();
}

ot::TextEditorCfg EntityBlockHierarchicalDocumentItem::createConfig(bool _includeData) {
	ot::TextEditorCfg cfg;
	cfg.setDocumentSyntax(ot::DocumentSyntax::PlainText);
	cfg.setEntityInformation(getBasicEntityInformation());
	cfg.setTitle(getName());
	
	if (_includeData) {
		cfg.setPlainText(getText());
	}

	return cfg;
}

ot::ContentChangedHandling EntityBlockHierarchicalDocumentItem::getTextContentChangedHandling() {
	return ot::ContentChangedHandling::ModelServiceSaves;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property accessors

void EntityBlockHierarchicalDocumentItem::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityBlock::addStorageData(_storage);

	_storage.append(
		bsoncxx::builder::basic::kvp("DocumentID", static_cast<int64_t>(m_documentUID)),
		bsoncxx::builder::basic::kvp("DocumentVersion", static_cast<int64_t>(m_documentVersion)),
		bsoncxx::builder::basic::kvp("DocumentType", m_documentType),
		bsoncxx::builder::basic::kvp("DocumentExtension", m_documentExtension)
	);
}

void EntityBlockHierarchicalDocumentItem::readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);

	m_documentUID = static_cast<ot::UID>(_docView["DocumentID"].get_int64());
	m_documentVersion = static_cast<ot::UID>(_docView["DocumentVersion"].get_int64());
	m_documentType = _docView["DocumentType"].get_utf8().value.data();
	m_documentExtension = _docView["DocumentExtension"].get_utf8().value.data();
}

void EntityBlockHierarchicalDocumentItem::ensureDocumentDataLoaded() {
	if (m_documentData) {
		return;
	}
	if (m_documentUID == ot::invalidUID) {
		OT_LOG_W("No document assigned to hierarchical document item entity { \"EntityID\": " + std::to_string(getEntityID()) + " }");
		return;
	}
	std::map<ot::UID, EntityBase*> tmp;
	std::unique_ptr<EntityBase> entityPtr(readEntityFromEntityIDAndVersion(nullptr, m_documentUID, m_documentVersion, tmp));
	if (!entityPtr) {
		OT_LOG_E("Failed to load document entity { \"EntityID\": " + std::to_string(m_documentUID) + ", \"Version\": " + std::to_string(m_documentVersion) + " } for hierarchical document item entity { \"EntityID\": " + std::to_string(getEntityID()) + " }");
		return;
	}
	EntityBinaryData* entityBinaryDataPtr = dynamic_cast<EntityBinaryData*>(entityPtr.get());

	if (!entityBinaryDataPtr) {
		OT_LOG_E("Loaded entity is not a data entity for hierarchical document item entity { \"EntityID\": " + std::to_string(getEntityID()) + ", \"DataID\": " + std::to_string(m_documentUID) + " }");
		return;
	}
	m_documentData.reset(entityBinaryDataPtr);
	entityPtr.release();
}
