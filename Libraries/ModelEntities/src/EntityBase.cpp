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
#include "EntityFactory.h"
#include "OTCore/String.h"

#include "OTCore/LogDispatcher.h"

_declspec(dllexport) DataStorageAPI::UniqueUIDGenerator *globalUidGenerator = nullptr;

EntityBase::EntityBase(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms) :
	m_initiallyHidden(false),
	m_parentEntity(_parent),
	m_observer(_obs),
	m_isModified(true),
	m_selectChildren(true),
	m_manageParentVisibility(true),
	m_manageChildVisibility(true),
	m_modelState(_ms),
	m_isDeletable(true)
{
	m_treeItem.setEntityID(_ID);
}

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

void EntityBase::setName(const std::string& _name) {
	m_treeItem.setEntityName(_name);
	setModified();
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

void EntityBase::setEntityID(ot::UID _id) {
	m_treeItem.setEntityID(_id);
	setModified();
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

void EntityBase::storeToDataBase(ot::UID _givenEntityVersion) {
	if (!getModified()) return;

	m_treeItem.setEntityVersion(_givenEntityVersion);
	entityIsStored();

	// This item collects all information about the entity and adds it to the storage data 
	auto doc = serialiseAsMongoDocument();

	DataBase::instance().storeDataItem(doc);

	resetModified();
}

void EntityBase::restoreFromDataBase(EntityBase *parent, EntityObserver *obs, ModelState *ms, const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) {
	setParent(parent);
	setObserver(obs);
	setModelState(ms);

	readSpecificDataFromDataBase(doc_view, entityMap);

	if (getEntityType() != DATA)
	{
		entityMap[getEntityID()] = this;
	}
}

void EntityBase::setTreeItem(const ot::EntityTreeItem& _treeItem, bool _resetTreeItemModified) {
	if (m_treeItem != _treeItem) {
		ot::UID uid = m_treeItem.getEntityID();
		ot::UID ver = m_treeItem.getEntityVersion();

		m_treeItem = _treeItem;
		
		m_treeItem.setEntityID(uid);
		m_treeItem.setEntityVersion(ver);
		
		if (_resetTreeItemModified) {
			m_treeItem.resetModified();
		}
	}
}

void EntityBase::setVisualizationTypes(const ot::VisualisationTypes& _types, bool _resetVisualizationTypesModified) {
	if (m_visualizationTypes != _types) {
		m_visualizationTypes = _types;

		if (_resetVisualizationTypesModified) {
			m_visualizationTypes.resetModified();
		}
	}
}

void EntityBase::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) {
	try {
		std::string schemaVersionKey = "SchemaVersion_" + getClassName();
		int schemaVersion = (int) DataBase::getIntFromView(doc_view, schemaVersionKey.c_str());
		if (schemaVersion != getSchemaVersion()) throw (std::exception());

		m_treeItem.setEntityID(DataBase::getIntFromView(doc_view, "EntityID"));
		m_treeItem.setEntityVersion(DataBase::getIntFromView(doc_view, "Version"));
		m_treeItem.setEntityName(std::string(doc_view["Name"].get_utf8().value.data()));
		m_initiallyHidden      = doc_view["initiallyHidden"].get_bool();
		auto bsonObj           = doc_view["Properties"].get_document();

		clearCallbacks(true);

		auto docIt = doc_view.find("Owner");
		if (docIt != doc_view.end()) {
			registerCallbacks(Callback::Properties | Callback::Selection | Callback::DataNotify, docIt->get_utf8().value.data(), true);
		}
		
		docIt = doc_view.find("Callbacks");
		if (docIt != doc_view.end()) {
			for (auto&& it : docIt->get_array().value) {
				auto kvp = it.get_document().view();
				CallbackFlags cb(DataBase::getIntFromView(kvp, "Callbacks"));
				std::string serviceName = kvp["ServiceName"].get_utf8().value.data();
				registerCallbacks(cb, serviceName, true);
			}
		}

		m_selectChildren = true;
		docIt = doc_view.find("selectChildren");
		if (docIt != doc_view.end()) {
			m_selectChildren = docIt->get_bool();
		}
		
		m_manageParentVisibility = true;
		docIt = doc_view.find("manageParentVisibility");
		if (docIt != doc_view.end()) {
			m_manageParentVisibility = docIt->get_bool();
		}
		
		m_manageChildVisibility = true;
		docIt = doc_view.find("manageChildVisibility");
		if (docIt != doc_view.end()) {
			m_manageChildVisibility = docIt->get_bool();
		}

		m_isDeletable = true;
		docIt = doc_view.find("isDeletable");
		if (docIt != doc_view.end()) {
			m_isDeletable = docIt->get_bool();
		}

		docIt = doc_view.find("isEditable");
		if (docIt != doc_view.end()) {
			m_treeItem.setIsEditable(docIt->get_bool());
		}

		docIt = doc_view.find("SelectTreeChilds");
		if (docIt != doc_view.end()) {
			m_treeItem.setSelectChilds(docIt->get_bool());
		}

		docIt = doc_view.find("VisibleTreeIcon");
		if (docIt != doc_view.end()) {
			m_treeItem.setVisibleIcon(docIt->get_string().value.data());
			if (m_treeItem.getIcons().getVisibleIcon().find('/') == std::string::npos) {
				// Old format without path, update to new format
				m_treeItem.setVisibleIcon("Default/" + m_treeItem.getIcons().getVisibleIcon());
			}
		}

		docIt = doc_view.find("HiddenTreeIcon");
		if (docIt != doc_view.end()) {
			m_treeItem.setHiddenIcon(docIt->get_string().value.data());
			if (m_treeItem.getIcons().getHiddenIcon().find('/') == std::string::npos) {
				// Old format without path, update to new format
				m_treeItem.setHiddenIcon("Default/" + m_treeItem.getIcons().getHiddenIcon());
			}
		}

		docIt = doc_view.find("VisualizationTypes");
		if (docIt != doc_view.end()) {
			m_visualizationTypes.setVisualisations(ot::VisualisationTypes::VisTypes(static_cast<uint64_t>(DataBase::getIntFromView(doc_view, "VisualizationTypes"))));
		}

		docIt = doc_view.find("CustomViewFlags");
		if (docIt != doc_view.end()) {
			m_visualizationTypes.clearCustomViewFlags();
			auto customViewFlagsArray = docIt->get_array().value;
			for (auto&& it : customViewFlagsArray) {
				auto kvp = it.get_document().view();
				ot::VisualisationTypes::VisualisationType visType = static_cast<ot::VisualisationTypes::VisualisationType>(static_cast<uint64_t>(DataBase::getIntFromView(kvp, "VisualisationType")));
				ot::WidgetViewBase::ViewFlags flags(static_cast<uint64_t>(DataBase::getIntFromView(kvp, "ViewFlags")));
				m_visualizationTypes.setCustomViewFlags(visType, flags);
			}
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

	bsoncxx::document::value bsonObj = bsoncxx::from_json(m_properties.createJSON(nullptr, false));

	// Prepare the callback array
	std::map<std::string, CallbackFlags> serviceCallbacksMap;
	for (const auto& pair : getCallbackData()) {
		Callback cb = pair.first;
		const std::list<std::string>& services = pair.second;
		for (const auto& serviceName : services) {
			auto it = serviceCallbacksMap.find(serviceName);
			if (it != serviceCallbacksMap.end()) {
				it->second |= cb;

			}
			else {
				serviceCallbacksMap.emplace(serviceName, cb);
			}
		}
	}

	// Fill the callback array
	bsoncxx::builder::basic::array callbackArray;

	for (const auto& pair : serviceCallbacksMap) {
		const std::string& serviceName = pair.first;
		CallbackFlags cb = pair.second;
		bsoncxx::builder::basic::document cbDoc;
		cbDoc.append(bsoncxx::builder::basic::kvp("ServiceName", serviceName));
		cbDoc.append(bsoncxx::builder::basic::kvp("Callbacks", static_cast<int64_t>(cb)));
		callbackArray.append(cbDoc.extract());
	}

	doc.append(bsoncxx::builder::basic::kvp("SchemaVersion_" + getClassName(), getSchemaVersion()),
		bsoncxx::builder::basic::kvp("EntityID", (long long)m_treeItem.getEntityID()),
		bsoncxx::builder::basic::kvp("Version", (long long)m_treeItem.getEntityVersion()),
		bsoncxx::builder::basic::kvp("Name", m_treeItem.getEntityName()),
		bsoncxx::builder::basic::kvp("isDeletable", m_isDeletable),
		bsoncxx::builder::basic::kvp("initiallyHidden", m_initiallyHidden),
		bsoncxx::builder::basic::kvp("selectChildren", m_selectChildren),
		bsoncxx::builder::basic::kvp("manageParentVisibility", m_manageParentVisibility),
		bsoncxx::builder::basic::kvp("manageChildVisibility", m_manageChildVisibility),
		bsoncxx::builder::basic::kvp("Properties", bsonObj),
		bsoncxx::builder::basic::kvp("Callbacks", callbackArray)
	);

	if (m_treeItem.getIsEditableChanged()){
		doc.append(bsoncxx::builder::basic::kvp("isEditable", m_treeItem.getIsEditable()));
	}
	if (m_treeItem.getSelectChildsChanged()) {
		doc.append(bsoncxx::builder::basic::kvp("SelectTreeChilds", m_treeItem.getSelectChilds()));
	}
	if (m_treeItem.getIconsChanged()) {
		doc.append(bsoncxx::builder::basic::kvp("VisibleTreeIcon", m_treeItem.getIcons().getVisibleIcon()));
		doc.append(bsoncxx::builder::basic::kvp("HiddenTreeIcon", m_treeItem.getIcons().getHiddenIcon()));
	}
	if (m_visualizationTypes.getVisualizationsModified()) {
		doc.append(bsoncxx::builder::basic::kvp("VisualizationTypes", static_cast<int64_t>(m_visualizationTypes.getVisualisations().underlying())));
	}
	if (m_visualizationTypes.getCustomViewFlagsModified()) {
		bsoncxx::builder::basic::array customViewFlagsArray;
		for (const auto& pair : m_visualizationTypes.getCustomViewFlags()) {
			ot::VisualisationTypes::VisualisationType visType = pair.first;
			ot::WidgetViewBase::ViewFlags flags = pair.second;
			bsoncxx::builder::basic::document cvfDoc;
			cvfDoc.append(bsoncxx::builder::basic::kvp("VisualisationType", static_cast<int64_t>(visType)));
			cvfDoc.append(bsoncxx::builder::basic::kvp("ViewFlags", static_cast<int64_t>(flags.underlying())));
			customViewFlagsArray.append(cvfDoc.extract());
		}
		doc.append(bsoncxx::builder::basic::kvp("CustomViewFlags", customViewFlagsArray));
	}

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


