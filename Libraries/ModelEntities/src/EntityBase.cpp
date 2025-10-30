// @otlicense
// File: EntityBase.cpp
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

#include "stdafx.h"

#include "EntityBase.h"
#include "DataBase.h"
#include "OldTreeIcon.h"
#include "EntityFactory.h"
#include "OTCore/String.h"

#include "OTCore/LogDispatcher.h"

_declspec(dllexport) DataStorageAPI::UniqueUIDGenerator *globalUidGenerator = nullptr;

EntityBase::EntityBase(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner) :
	m_entityID(_ID),
	m_entityStorageVersion(0),
	m_initiallyHidden(false),
	m_isEditable(false),
	m_parentEntity(_parent),
	m_observer(_obs),
	m_isModified(true),
	m_selectChildren(true),
	m_manageParentVisibility(true),
	m_manageChildVisibility(true),
	m_modelState(_ms),
	m_owningService(_owner),
	m_name(""),
	m_isDeletable(true)
{}

EntityBase::~EntityBase() {
	if (m_parentEntity != nullptr) {
		m_parentEntity->removeChild(this);
	}
	if (m_observer != nullptr) {
		m_observer->entityRemoved(this);
	}
}

void EntityBase::setUidGenerator(DataStorageAPI::UniqueUIDGenerator *_uidGenerator) {
	if (globalUidGenerator == nullptr) {
		globalUidGenerator = _uidGenerator;
	}
	else {
		assert(globalUidGenerator == _uidGenerator);
	}
}

DataStorageAPI::UniqueUIDGenerator *EntityBase::getUidGenerator(void) {
	return globalUidGenerator;
}

std::string EntityBase::getNameOnly() const {
	std::list<std::string> tmp = ot::String::split(this->getName(), '/', true);
	if (tmp.empty()) {
		return this->getName();
	}
	else {
		return tmp.back();
	}
}

void EntityBase::setModified(void) {
	if (m_observer != nullptr) m_observer->entityModified(this);
	
	m_isModified = true;
}

bool EntityBase::updateFromProperties(void) {
	// This is the standard base class handler. Therefore no specific update code is provided for this entity.
	// We check whether there are any property updates needed (which should not be the case).
	assert(!getProperties().anyPropertyNeedsUpdate());

	return false; // No property grid update necessary
}

void EntityBase::storeToDataBase(void) {
	if (!getModified()) {
		return;
	}

	assert(globalUidGenerator != nullptr);
	if (globalUidGenerator == nullptr) {
		return;
	}

	ot::UID entityVersion = globalUidGenerator->getUID();

	storeToDataBase(entityVersion);
}

void EntityBase::storeToDataBase(ot::UID givenEntityVersion) {
	if (!getModified()) return;

	m_entityStorageVersion = givenEntityVersion;
	entityIsStored();

	// This item collects all information about the entity and adds it to the storage data 
	auto doc = serialiseAsMongoDocument();

	DataBase::instance().storeDataItem(doc);

	resetModified();
}

void EntityBase::restoreFromDataBase(EntityBase *parent, EntityObserver *obs, ModelState *ms, bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) {
	setParent(parent);
	setObserver(obs);
	setModelState(ms);

	readSpecificDataFromDataBase(doc_view, entityMap);

	entityMap[getEntityID()] = this;
}

void EntityBase::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) {
	try {
		std::string schemaVersionKey = "SchemaVersion_" + getClassName();
		int schemaVersion = (int) DataBase::getIntFromView(doc_view, schemaVersionKey.c_str());
		if (schemaVersion != getSchemaVersion()) throw (std::exception());

		m_entityID             = DataBase::getIntFromView(doc_view, "EntityID");
		m_entityStorageVersion = DataBase::getIntFromView(doc_view, "Version");
		m_name                 = doc_view["Name"].get_utf8().value.data();
		m_initiallyHidden      = doc_view["initiallyHidden"].get_bool();
		auto bsonObj           = doc_view["Properties"].get_document();

		m_owningService = "Model";
		try {
			m_owningService = doc_view["Owner"].get_utf8().value.data();
		}
		catch (std::exception)
		{
		}

		m_isEditable = false;
		try {
			m_isEditable = doc_view["isEditable"].get_bool();
		}
		catch (std::exception)
		{
		}

		m_selectChildren = true;
		try {
			m_selectChildren = doc_view["selectChildren"].get_bool();
		}
		catch (std::exception)
		{
		}

		m_manageParentVisibility = true;
		try {
			m_manageParentVisibility = doc_view["manageParentVisibility"].get_bool();
		}
		catch (std::exception)
		{
		}

		m_manageChildVisibility = true;
		try {
			m_manageChildVisibility = doc_view["manageChildVisibility"].get_bool();
		}
		catch (std::exception)
		{
		}

		m_isDeletable = true;
		if (doc_view.find("isDeletable") != doc_view.end()) {
			m_isDeletable = doc_view["isDeletable"].get_bool();
		}

		std::string propertiesJSON = bsoncxx::to_json(bsonObj);
		m_properties.buildFromJSON(propertiesJSON, nullptr);
		m_properties.forceResetUpdateForAllProperties();
		
		resetModified();
	}
	catch (std::exception _e) {
		OT_LOG_EAS(_e.what()); // Read failed
	}
}

void EntityBase::addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds) {
	if (getEntityID() > 0) {
		prefetchIds.push_back(getEntityID());
	}
}

ot::UID EntityBase::createEntityUID(void) {
	assert(getUidGenerator() != nullptr);

	return getUidGenerator()->getUID();
}

EntityBase *EntityBase::readEntityFromEntityIDAndVersion(EntityBase *parent, ot::UID entityID, ot::UID version, std::map<ot::UID, EntityBase *> &entityMap) {
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(entityID, version, doc)) {
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase* entity = EntityFactory::instance().create(entityType);

	if (entity == nullptr) {
		return nullptr;
	}

	entity->restoreFromDataBase(parent, getObserver(), getModelState(), doc_view, entityMap);

	return entity;
}

EntityBase *EntityBase::readEntityFromEntityID(EntityBase *parent, ot::UID entityID, std::map<ot::UID, EntityBase *> &entityMap) {
	assert(m_modelState != nullptr);
	ot::UID version = m_modelState->getCurrentEntityVersion(entityID);

	return readEntityFromEntityIDAndVersion(parent, entityID, version, entityMap);
}

ot::UID EntityBase::getCurrentEntityVersion(ot::UID entityID) {
	assert(m_modelState != nullptr);
	return m_modelState->getCurrentEntityVersion(entityID);
}

void EntityBase::entityIsStored(void) {
	// Determined the parent ID
	ot::UID parentID = 0;

	if (getParent() != nullptr) {
		parentID = getParent()->getEntityID();
		assert(parentID != 0);
	}

	assert(getEntityID() != 0);
	assert(getEntityStorageVersion() > 0);

	ModelStateEntity::tEntityType entityType = ModelStateEntity::tEntityType::DATA;

	switch (getEntityType()) {
	case EntityBase::TOPOLOGY:
		entityType = ModelStateEntity::tEntityType::TOPOLOGY;
		break;
	case EntityBase::DATA:
		entityType = ModelStateEntity::tEntityType::DATA;
		break;
	default:
		assert(0); // Unknown entity type
	}

	if (m_modelState != nullptr) {
		m_modelState->storeEntity(getEntityID(), parentID, getEntityStorageVersion(), entityType);
	}
}

bsoncxx::builder::basic::document EntityBase::serialiseAsMongoDocument()
{
	auto doc = bsoncxx::builder::basic::document{};

	doc.append(bsoncxx::builder::basic::kvp("SchemaType", getClassName()));

	assert(!m_owningService.empty());

	bsoncxx::document::value bsonObj = bsoncxx::from_json(m_properties.createJSON(nullptr, false));

	doc.append(bsoncxx::builder::basic::kvp("SchemaVersion_" + getClassName(), getSchemaVersion()),
		bsoncxx::builder::basic::kvp("EntityID", (long long)m_entityID),
		bsoncxx::builder::basic::kvp("Version", (long long)m_entityStorageVersion),
		bsoncxx::builder::basic::kvp("Name", m_name),
		bsoncxx::builder::basic::kvp("isDeletable", m_isDeletable),
		bsoncxx::builder::basic::kvp("initiallyHidden", m_initiallyHidden),
		bsoncxx::builder::basic::kvp("isEditable", m_isEditable),
		bsoncxx::builder::basic::kvp("selectChildren", m_selectChildren),
		bsoncxx::builder::basic::kvp("manageParentVisibility", m_manageParentVisibility),
		bsoncxx::builder::basic::kvp("manageChildVisibility", m_manageChildVisibility),
		bsoncxx::builder::basic::kvp("Owner", m_owningService),
		bsoncxx::builder::basic::kvp("Properties", bsonObj)
	);

	addStorageData(doc);

	return doc;
}

void EntityBase::detachFromHierarchy(void) {
	// Here we detach the entity from the hierarcy which means detaching from the parent.
	if (getParent() != nullptr) {
		getParent()->removeChild(this);
		setParent(nullptr);
	}
}


