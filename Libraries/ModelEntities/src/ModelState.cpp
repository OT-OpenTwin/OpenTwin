#include "stdafx.h"

#undef max
#undef min

#include "ModelState.h"
#include "OTCore/Logger.h"

#include "DataBase.h"
#include "EntityBase.h"
#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccessBase.h"
#include "Helper\QueryBuilder.h"
#include "InvalidUID.h"

#include <fstream>

ModelState::ModelState(unsigned int sessionID, unsigned int serviceID) :
	m_stateModified(false),
	m_maxNumberArrayEntitiesPerState(250000)
{
	DataStorageAPI::UniqueUIDGenerator *uidGenerator = EntityBase::getUidGenerator();
	if (uidGenerator == nullptr)
	{
		uidGenerator = new DataStorageAPI::UniqueUIDGenerator(sessionID, serviceID);
	}

	m_uniqueUIDGenerator = uidGenerator;
	EntityBase::setUidGenerator(m_uniqueUIDGenerator);
}

ModelState::~ModelState()
{
}

// ###########################################################################################################################################################################################################################################################################################################################

// General

void ModelState::reset(void)
{
	m_graphCfg.clear();
	m_currentModelBaseStateVersion.clear();

	m_entities.clear();
	m_addedOrModifiedEntities.clear();
	m_removedEntities.clear();
	m_entityChildrenList.clear();

	m_stateModified = false;
}

bool ModelState::openProject(void) {
	// load the version graph
	loadVersionGraph();

	// Load the model entity to determine the currently active branch and version
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< bsoncxx::builder::stream::finalize;

	auto emptyFilterDoc = bsoncxx::builder::basic::document{};

	auto sortDoc = bsoncxx::builder::basic::document{};
	sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

	auto result = docBase.GetDocument(std::move(queryDoc), std::move(emptyFilterDoc.extract()), std::move(sortDoc.extract()));
	if (!result) return false;  // No model entity found

	std::string activeBranch = result->view()["ActiveBranch"].get_utf8().value.data();
	std::string activeVersion = result->view()["ActiveVersion"].get_utf8().value.data();

	m_activeBranchInModelEntity = activeBranch;
	m_activeVersionInModelEntity = activeVersion;

	// Activate the branch
	m_graphCfg.setActiveBranchName(activeBranch);

	// If the active Version is empty, the last version in the active branch is active
	if (activeVersion.empty()) {
		activeVersion = getLastVersionInActiveBranch();
	}

	// load the currently active version
	return loadModelState(activeVersion);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Entity handling

unsigned long long ModelState::createEntityUID(void)
{
	return m_uniqueUIDGenerator->getUID();
}

void ModelState::storeEntity(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentEntityID, ModelStateEntity::EntityVersion entityVersion, ModelStateEntity::tEntityType entityType)
{
	if (m_entities.count(entityID) == 0)
	{
		// We have a new entity to store
		addNewEntity(entityID, parentEntityID, entityVersion, entityType);
	}
	else
	{
		modifyEntity(entityID, parentEntityID, entityVersion, entityType);
	}
}

void ModelState::addNewEntity(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentEntityID, ModelStateEntity::EntityVersion entityVersion, ModelStateEntity::tEntityType entityType)
{
	ModelStateEntity newEntity;
	newEntity.setParentEntityID(parentEntityID);
	newEntity.setVersion(entityVersion);
	newEntity.setEntityType(entityType);

	// Add the new entity to the parent / child list
	addEntityToParent(entityID, parentEntityID);

	assert(m_entities.count(entityID) == 0); // Ensure that the entity does not already exist
	m_entities[entityID] = newEntity;
	m_addedOrModifiedEntities[entityID] = newEntity;
	m_stateModified = true;
}

void ModelState::modifyEntity(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentEntityID, ModelStateEntity::EntityVersion entityVersion, ModelStateEntity::tEntityType entityType)
{
	if (m_entities.count(entityID) > 0)
	{
		ModelStateEntity::EntityID previousParentID = m_entities[entityID].getParentEntityID();

		// This entity is a topology entity
		ModelStateEntity modifiedEntity = m_entities[entityID];
		modifiedEntity.setVersion(entityVersion);
		modifiedEntity.setParentEntityID(parentEntityID);
		modifiedEntity.setEntityType(entityType);

		if (previousParentID != parentEntityID)
		{
			// The parent has changed
			removeEntityFromParent(entityID, previousParentID);
			addEntityToParent(entityID, parentEntityID);
		}

		m_entities[entityID] = modifiedEntity;
		m_addedOrModifiedEntities[entityID] = modifiedEntity;
		m_stateModified = true;
	}
	else
	{
		assert(0); // The entity was not found
	}
}

void ModelState::modifyEntityVersion(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityVersion entityVersion)
{
	if (m_entities.count(entityID) > 0)
	{
		// This entity is a topology entity
		ModelStateEntity modifiedEntity = m_entities[entityID];
		modifiedEntity.setVersion(entityVersion);

		m_entities[entityID] = modifiedEntity;
		m_addedOrModifiedEntities[entityID] = modifiedEntity;
		m_stateModified = true;
	}
	else
	{
		assert(0); // The entity was not found
	}
}

void ModelState::modifyEntityParent(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentEntityID)
{
	if (m_entities.count(entityID) > 0)
	{
		// This entity is a topology entity
		ModelStateEntity modifiedEntity = m_entities[entityID];
		modifiedEntity.setParentEntityID(parentEntityID);

		m_entities[entityID] = modifiedEntity;
		m_addedOrModifiedEntities[entityID] = modifiedEntity;
		m_stateModified = true;
	}
	else
	{
		assert(0); // The entity was not found
	}
}

void ModelState::removeEntity(ModelStateEntity::EntityID entityID, bool considerChildren)
{
	//std::cout << "remove entity: " << entityID << std::endl;

	if (m_entities.count(entityID) > 0)
	{
		// This entity is a topology entity
		m_removedEntities[entityID] = m_entities[entityID];
		m_entities.erase(entityID);
		m_stateModified = true;
	}
	else
	{
		// The entity was not found. This might happen if entities are deleted which have not been added to the model previously
		return;
	}

	// Now remove the children of the entity
	if (considerChildren)
	{

		if (m_entityChildrenList.count(entityID) > 0)
		{
			std::list<ModelStateEntity::EntityID> children = m_entityChildrenList[entityID];

			for (auto child : children)
			{
				removeEntity(child);
			}

			// And finally remove the children information for the current entity
			m_entityChildrenList.erase(entityID);
		}
	}
}

bool ModelState::loadModelState(const std::string& _version)
{
	OT_LOG_D("Loading model state { \"Version\": \"" + _version + "\" }");

	if (!isVersionInActiveBranch(_version))
	{
		activateBranch(_version);
	}

	// Now we search for an entity with SchemaType "ModelState" and the given version
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::basic::document{};
	queryDoc.append(bsoncxx::builder::basic::kvp("SchemaType", "ModelState"));
	queryDoc.append(bsoncxx::builder::basic::kvp("Version", _version));

	auto filterDoc = bsoncxx::builder::basic::document{};

	auto result = docBase.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));
	if (!result) {
		OT_LOG_E("Model state not found { \"Version\": \"" + _version + "\" }");
		return false; // Model state not found
	}

	clearChildrenInformation();

	if (!loadModelFromDocument(result->view()))
	{
		// The model state could not be loaded
		OT_LOG_E("Failed to load model state { \"Version\": \"" + _version + "\" }");
		return false;
	}

	storeCurrentVersionInModelEntity();

	buildChildrenInformation();

	storeCurrentVersionInModelEntity();

	return true;
}

void ModelState::clearChildrenInformation(void)
{
	m_entityChildrenList.clear();
}

void ModelState::buildChildrenInformation(void)
{
	m_entityChildrenList.clear();

	for (auto entity : m_entities)
	{
		addEntityToParent(entity.first, entity.second.getParentEntityID());
	}
}

void ModelState::addEntityToParent(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentID)
{
	if (m_entityChildrenList.count(parentID) > 0)
	{
		m_entityChildrenList.find(parentID)->second.push_back(entityID);
	}
	else
	{
		std::list<ModelStateEntity::EntityID> childList;
		childList.push_back(entityID);

		m_entityChildrenList[parentID] = childList;
	}
}

void ModelState::removeEntityFromParent(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentID)
{
	if (m_entityChildrenList.count(parentID) > 0)
	{
		m_entityChildrenList.find(parentID)->second.remove(entityID);
	}
}

bool ModelState::saveModelState(bool forceSave, bool forceAbsoluteState, const std::string &saveComment) {
	if (!m_stateModified && !forceSave) {
		// Nothing to save. The model state has not changed.
		return true;
	}

	// Store the current version as the parent version
	std::string parentVersion = m_graphCfg.getActiveVersionName();

	// Remove all entities which are newer than the current model state, but older than the last inactive model state or model
	// state extension. Such entries appear after a previous undo operation.
	if (canRedo()) {
		// We have redo information available. To avoid conflicts, we need to create a new branch
		createAndActivateNewBranch();
	}

	size_t numberArrayEntitiesAbsolute = 3 * m_entities.size();
	size_t numberArrayEntitiesRelative = 3 * m_addedOrModifiedEntities.size() + m_removedEntities.size();

	// We perform a relative save if the size of the relative state is five times less than the size of the absolute state
	bool saveAbsolute = (numberArrayEntitiesRelative * 5 > numberArrayEntitiesAbsolute);

	if (numberArrayEntitiesRelative > m_maxNumberArrayEntitiesPerState) {
		// In case that we need to store extension documents, we store an absolute state
		saveAbsolute = true;
	}

	if (forceAbsoluteState) {
		// We force saving as an absolute state
		saveAbsolute = true;
	}

	if (m_currentModelBaseStateVersion.empty()) {
		// We can not save a relative state without having the base state
		saveAbsolute = true;
	}

	// Now we need to increment our version counter
	m_graphCfg.incrementActiveVersion();

	// Now add the new state to the graph
	m_graphCfg.insertVersion(m_graphCfg.getActiveVersionName(), parentVersion, "", saveComment);

	// Finally we store an absolute or relative state 
	if (saveAbsolute) {
		if (!saveAbsoluteState(saveComment)) {
			return false;
		}
	}
	else {
		if (!saveIncrementalState(saveComment)) {
			return false;
		}
	}

	// The document is up-to-date
	m_stateModified = false;

	// We need to remove the modification information
	m_addedOrModifiedEntities.clear();
	m_removedEntities.clear();

	this->storeCurrentVersionInModelEntity();

	return true;
}

ModelStateEntity::EntityVersion ModelState::getCurrentEntityVersion(ModelStateEntity::EntityID entityID)
{
	// If the entity is not part of the current state, return invalid UID
	if (m_entities.count(entityID) == 0)
	{
		return ot::getInvalidUID();
	}

	// Ensure that the entity exists
	assert(m_entities.count(entityID) > 0);

	// Now return the current version of the entity
	return m_entities[entityID].getEntityVersion();
}

ModelStateEntity::EntityID ModelState::getCurrentEntityParent(ModelStateEntity::EntityID entityID)
{
	// Ensure that the entity exists
	assert(m_entities.count(entityID) > 0);

	// Now return the current version of the entity
	return m_entities[entityID].getParentEntityID();
}

void ModelState::getListOfTopologyEntites(std::list<unsigned long long> &topologyEntities)
{
	// Loop through all entities and add the topology entities to the list
	for (auto entity : m_entities)
	{
		if (entity.second.getEntityType() == ModelStateEntity::tEntityType::TOPOLOGY)
		{
			// This entity is a topology entits, so add its id to the list
			topologyEntities.push_back(entity.first);
		}
	}
}

bool ModelState::loadModelFromDocument(bsoncxx::document::view docView)
{
	std::string storageType = docView["Type"].get_utf8().value.data();

	// The current document can either be absolute or relative
	if (storageType == "absolute")
	{
		// We have an absolute model state document

		// The corresponding base state is our own state
		m_currentModelBaseStateVersion = docView["Version"].get_utf8().value.data();

		return loadAbsoluteState(docView);
	}
	else if (storageType == "relative")
	{
		ot::VersionGraphVersionCfg incrementalStateVersion(docView["Version"].get_utf8().value.data());

		m_currentModelBaseStateVersion = docView["BaseState"].get_utf8().value.data();

		// Find and load the last absolute state
		if (!loadModelState(m_currentModelBaseStateVersion)) return false;
		
		// Now load the following (incremental) versions until we reach the desired version
		DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

		
		std::list<const ot::VersionGraphVersionCfg*> versionsToImport = m_graphCfg.findNextVersions(m_currentModelBaseStateVersion, incrementalStateVersion.getBranchName(), incrementalStateVersion.getName());

		for (const ot::VersionGraphVersionCfg* versionData : versionsToImport) {
			auto queryDoc = bsoncxx::builder::basic::document{};
			queryDoc.append(bsoncxx::builder::basic::kvp("SchemaType", "ModelState"));
			queryDoc.append(bsoncxx::builder::basic::kvp("Version", versionData->getName()));

			auto filterDoc = bsoncxx::builder::basic::document{};

			auto result = docBase.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));
			if (!result) {
				OT_LOG_EAS("Model state not found { \"Version\": \"" + incrementalStateVersion.getName() + "\" }");
				return false; // Model state not found
			}

			this->loadIncrementalState(result->view());
		}

		return true;
	}
	else
	{
		OT_LOG_EA("Unknown storage type");
		return false;
	}
}

void ModelState::clearModelState(void)
{
	m_graphCfg.setActiveVersionName("");

	m_entities.clear();

	m_addedOrModifiedEntities.clear();
	m_removedEntities.clear();

	m_stateModified = false;
}

bool ModelState::loadAbsoluteState(bsoncxx::document::view docView)
{
	clearModelState();

	return loadState(docView, "absolute");
}

bool ModelState::loadIncrementalState(bsoncxx::document::view docView)
{
	return loadState(docView, "relative");
}

bool ModelState::loadState(bsoncxx::document::view docView, const std::string &expectedType)
{
	if (docView["SchemaType"].get_utf8().value.data() != std::string("ModelState")) {
		OT_LOG_EAS("ShemaType \"ModelState\" not found");
	}
	if (docView["SchemaVersion_ModelState"].get_int32() != 1) {
		OT_LOG_EAS("Invalid \"SchemaVersion_ModelState\": (" + std::to_string(docView["SchemaVersion_ModelState"].get_int32()) + ")");
	}
	if (docView["Type"].get_utf8().value.data() != expectedType) {
		OT_LOG_EAS("Invalid type { \"ExpectedType\": \"" + expectedType + "\", \"ActualType\": \"" + docView["Type"].get_utf8().value.data() + "\" }");
	}

	m_graphCfg.setActiveVersionName(docView["Version"].get_utf8().value.data());

	bool extensionState = docView["Extension"].get_bool();

	loadStateData(docView);

	// Check whether there are extension documents and if so, load them
	if (extensionState)
	{
		// We now search for all state extension documents for the current state version
		auto queryDoc = bsoncxx::builder::stream::document{}
			<< "SchemaType" << "ModelStateExtension"
			<< "Version" << m_graphCfg.getActiveVersionName()
			<< bsoncxx::builder::stream::finalize;

		auto filterDoc = bsoncxx::builder::basic::document{};

		DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());
		auto resultList = docBase.GetAllDocument(std::move(queryDoc), std::move(filterDoc.extract()), 0);

		// Now we need to process all extension states
		for (auto result : resultList)
		{
			loadStateData(result);
		}
	}

	// The state has not been modified compared to the stored (and named) version
	m_stateModified = false;

	return true;
}

void ModelState::loadStateData(bsoncxx::document::view docView)
{
	bsoncxx::document::element topoDel_Id = docView["topoDel_Id"];

	if (topoDel_Id)
	{
		auto id = topoDel_Id.get_array().value;

		size_t numberElements = std::distance(id.begin(), id.end());

		auto cId = id.begin();

		for (unsigned long index = 0; index < numberElements; index++)
		{
			m_entities.erase(cId->get_int64());
			cId++;
		}
	}

	bsoncxx::document::element dataDel_Id = docView["dataDel_Id"];

	if (dataDel_Id)
	{
		auto id = dataDel_Id.get_array().value;

		size_t numberElements = std::distance(id.begin(), id.end());

		auto cId = id.begin();

		for (unsigned long index = 0; index < numberElements; index++)
		{
			m_entities.erase(cId->get_int64());
			cId++;
		}
	}

	bsoncxx::document::element topoAddEdit_Id = docView["topoAddEdit_Id"];
	bsoncxx::document::element topoAddEdit_Parent = docView["topoAddEdit_Parent"];
	bsoncxx::document::element topoAddEdit_Version = docView["topoAddEdit_Version"];

	if (topoAddEdit_Id && topoAddEdit_Parent && topoAddEdit_Version)
	{
		auto id      = topoAddEdit_Id.get_array().value;
		auto parent  = topoAddEdit_Parent.get_array().value;
		auto version = topoAddEdit_Version.get_array().value;

		size_t numberElements = std::distance(id.begin(), id.end());
		assert(numberElements == std::distance(parent.begin(), parent.end()));
		assert(numberElements == std::distance(version.begin(), version.end()));

		auto cId = id.begin();
		auto cParent = parent.begin();
		auto cVersion = version.begin();

		ModelStateEntity entity;

		for (unsigned long index = 0; index < numberElements; index++)
		{
			entity.setParentEntityID(cParent->get_int64());
			entity.setVersion(cVersion->get_int64());
			entity.setEntityType(ModelStateEntity::tEntityType::TOPOLOGY);

			m_entities[cId->get_int64()] = entity;

			cId++;
			cParent++;
			cVersion++;
		}
	}

	bsoncxx::document::element dataAddEdit_Id = docView["dataAddEdit_Id"];
	bsoncxx::document::element dataAddEdit_Parent = docView["dataAddEdit_Parent"];
	bsoncxx::document::element dataAddEdit_Version = docView["dataAddEdit_Version"];

	if (dataAddEdit_Id && dataAddEdit_Parent && dataAddEdit_Version)
	{
		auto id      = dataAddEdit_Id.get_array().value;
		auto parent  = dataAddEdit_Parent.get_array().value;
		auto version = dataAddEdit_Version.get_array().value;

		size_t numberElements = std::distance(id.begin(), id.end());
		assert(numberElements == std::distance(parent.begin(), parent.end()));
		assert(numberElements == std::distance(version.begin(), version.end()));

		auto cId = id.begin();
		auto cParent = parent.begin();
		auto cVersion = version.begin();

		ModelStateEntity entity;

		for (unsigned long index = 0; index < numberElements; index++)
		{
			entity.setParentEntityID(cParent->get_int64());
			entity.setVersion(cVersion->get_int64());
			entity.setEntityType(ModelStateEntity::tEntityType::DATA);

			m_entities[cId->get_int64()] = entity;

			cId++;
			cParent++;
			cVersion++;
		}
	}
}

bool ModelState::saveAbsoluteState(const std::string &saveComment)
{
	// Count entities and check whether we need an extension document
	size_t numberEntities = 3 * m_entities.size();

	bool extension = (numberEntities > m_maxNumberArrayEntitiesPerState);

	if (extension)
	{
		return saveAbsoluteStateWithExtension(saveComment);
	}

	// Create a document and write the header information
	auto doc = bsoncxx::builder::basic::document{};

	doc.append(bsoncxx::builder::basic::kvp("SchemaType", "ModelState"));
	doc.append(bsoncxx::builder::basic::kvp("SchemaVersion_ModelState", (int)1));
	doc.append(bsoncxx::builder::basic::kvp("Version", m_graphCfg.getActiveVersionName()));
	doc.append(bsoncxx::builder::basic::kvp("ParentVersion", getPreviousVersion(m_graphCfg.getActiveVersionName())));
	doc.append(bsoncxx::builder::basic::kvp("Type", "absolute"));
	doc.append(bsoncxx::builder::basic::kvp("Extension", false));
	doc.append(bsoncxx::builder::basic::kvp("Description", saveComment));

	if (!m_entities.empty())
	{
		// Add the entities (if any)

		auto topo_id = bsoncxx::builder::basic::array();
		auto topo_parent = bsoncxx::builder::basic::array();
		auto topo_version = bsoncxx::builder::basic::array();

		auto data_id = bsoncxx::builder::basic::array();
		auto data_parent = bsoncxx::builder::basic::array();
		auto data_version = bsoncxx::builder::basic::array();

		bool hasTopoEntities = false;
		bool hasDataEntities = false;

		for (auto entity : m_entities)
		{
			switch (entity.second.getEntityType())
			{
			case ModelStateEntity::tEntityType::TOPOLOGY:
				topo_id.append((long long)entity.first);
				topo_parent.append((long long)entity.second.getParentEntityID());
				topo_version.append((long long)entity.second.getEntityVersion());
				hasTopoEntities = true;
				break;
			case ModelStateEntity::tEntityType::DATA:
				data_id.append((long long)entity.first);
				data_parent.append((long long)entity.second.getParentEntityID());
				data_version.append((long long)entity.second.getEntityVersion());
				hasDataEntities = true;
				break;
			default:
				// Unknown type
				assert(0);
			}
		}		
		
		if (hasTopoEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Id", topo_id));
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Parent", topo_parent));
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Version", topo_version));
		}

		if (hasDataEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Id", data_id));
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Parent", data_parent));
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Version", data_version));
		}
	}

	// Store the document in the data base

	//int MaxDocumentLength = 16777216;
	//auto docDataLength = doc.view().length();
	//assert(docDataLength < MaxDocumentLength);

	DataBase::GetDataBase()->StorePlainDataItem(doc);

	// The current version becomes the new base state
	m_currentModelBaseStateVersion = m_graphCfg.getActiveVersionName();
	
	return true;
}

bool ModelState::saveAbsoluteStateWithExtension(const std::string &saveComment)
{
	// Count entities and check whether we need an extension document
	size_t numberEntities = 3 * m_entities.size();

	bool extension = (numberEntities > m_maxNumberArrayEntitiesPerState);
	assert(extension);

	std::map<ModelStateEntity::EntityID, ModelStateEntity> entitiesLeft = m_entities;

	writeMainDocument(entitiesLeft, saveComment);

	while (!entitiesLeft.empty())
	{
		writeExtensionDocument(entitiesLeft);
	}

	// The current version becomes the new base state
	m_currentModelBaseStateVersion = m_graphCfg.getActiveVersionName();

	return true;
}

bool ModelState::writeMainDocument(std::map<ModelStateEntity::EntityID, ModelStateEntity> &entitiesLeft, const std::string &saveComment)
{
	// Create a document and write the header information
	auto doc = bsoncxx::builder::basic::document{};

	doc.append(bsoncxx::builder::basic::kvp("SchemaType", "ModelState"));
	doc.append(bsoncxx::builder::basic::kvp("SchemaVersion_ModelState", (int)1));
	doc.append(bsoncxx::builder::basic::kvp("Version", m_graphCfg.getActiveVersionName()));
	doc.append(bsoncxx::builder::basic::kvp("ParentVersion", getPreviousVersion(m_graphCfg.getActiveVersionName())));
	doc.append(bsoncxx::builder::basic::kvp("Type", "absolute"));
	doc.append(bsoncxx::builder::basic::kvp("Extension", true));
	doc.append(bsoncxx::builder::basic::kvp("Description", saveComment));

	size_t numberArrayEntriesWritten = 0;

	// Add the remaining entities (if any)
	if (!entitiesLeft.empty())
	{
		// Store the entities as arrays in the document (id, parent, version)

		auto topo_id = bsoncxx::builder::basic::array();
		auto topo_parent = bsoncxx::builder::basic::array();
		auto topo_version = bsoncxx::builder::basic::array();

		auto data_id = bsoncxx::builder::basic::array();
		auto data_parent = bsoncxx::builder::basic::array();
		auto data_version = bsoncxx::builder::basic::array();

		std::map<ModelStateEntity::EntityID, ModelStateEntity> entitiesLocal = entitiesLeft;

		bool hasTopoEntities = false;
		bool hasDataEntities = false;

		for (auto entity : m_entities)
		{
			if (numberArrayEntriesWritten > m_maxNumberArrayEntitiesPerState) break;

			entitiesLocal.erase((long long)entity.first);

			switch (entity.second.getEntityType())
			{
			case ModelStateEntity::tEntityType::TOPOLOGY:
				topo_id.append((long long)entity.first);
				topo_parent.append((long long)entity.second.getParentEntityID());
				topo_version.append((long long)entity.second.getEntityVersion());
				hasTopoEntities = true;
				break;
			case ModelStateEntity::tEntityType::DATA:
				data_id.append((long long)entity.first);
				data_parent.append((long long)entity.second.getParentEntityID());
				data_version.append((long long)entity.second.getEntityVersion());
				hasDataEntities = true;
				break;
			default:
				// Unknown type
				assert(0);
			}

			numberArrayEntriesWritten += 3;
		}

		if (hasTopoEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Id", topo_id));
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Parent", topo_parent));
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Version", topo_version));
		}

		if (hasDataEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Id", data_id));
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Parent", data_parent));
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Version", data_version));
		}
	}

	// Store the document in the data base
	DataBase::GetDataBase()->StorePlainDataItem(doc);

	return true;
}

bool ModelState::writeExtensionDocument(std::map<ModelStateEntity::EntityID, ModelStateEntity> &entitiesLeft)
{
	// Create a document and write the header information
	auto doc = bsoncxx::builder::basic::document{};

	doc.append(bsoncxx::builder::basic::kvp("SchemaType", "ModelStateExtension"));
	doc.append(bsoncxx::builder::basic::kvp("SchemaVersion_ModelStateExtension", (int)1));
	doc.append(bsoncxx::builder::basic::kvp("Version", m_graphCfg.getActiveVersionName()));

	size_t numberArrayEntriesWritten = 0;

	// Add the remaining entities (if any)
	if (!entitiesLeft.empty())
	{
		// Store the entities as arrays in the document (id, parent, version)

		auto topo_id = bsoncxx::builder::basic::array();
		auto topo_parent = bsoncxx::builder::basic::array();
		auto topo_version = bsoncxx::builder::basic::array();

		auto data_id = bsoncxx::builder::basic::array();
		auto data_parent = bsoncxx::builder::basic::array();
		auto data_version = bsoncxx::builder::basic::array();

		std::map<ModelStateEntity::EntityID, ModelStateEntity> entitiesLocal = entitiesLeft;

		bool hasTopoEntities = false;
		bool hasDataEntities = false;

		for (auto entity : entitiesLocal)
		{
			if (numberArrayEntriesWritten > m_maxNumberArrayEntitiesPerState) break;

			entitiesLocal.erase((long long)entity.first);

			switch (entity.second.getEntityType())
			{
			case ModelStateEntity::tEntityType::TOPOLOGY:
				topo_id.append((long long)entity.first);
				topo_parent.append((long long)entity.second.getParentEntityID());
				topo_version.append((long long)entity.second.getEntityVersion());
				hasTopoEntities = true;
				break;
			case ModelStateEntity::tEntityType::DATA:
				data_id.append((long long)entity.first);
				data_parent.append((long long)entity.second.getParentEntityID());
				data_version.append((long long)entity.second.getEntityVersion());
				hasDataEntities = true;
				break;
			default:
				// Unknown type
				assert(0);
			}

			numberArrayEntriesWritten += 3;
		}

		if (hasTopoEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Id", topo_id));
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Parent", topo_parent));
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Version", topo_version));
		}

		if (hasDataEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Id", data_id));
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Parent", data_parent));
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Version", data_version));
		}
	}

	// Store the document in the data base
	DataBase::GetDataBase()->StorePlainDataItem(doc);

	return true;
}

bool ModelState::saveIncrementalState(const std::string &saveComment)
{
	// Count entities and check whether we need an extension document
	size_t numberEntities = 3 * m_addedOrModifiedEntities.size() + m_removedEntities.size();

	bool extension = (numberEntities > m_maxNumberArrayEntitiesPerState);
	assert(!extension); // We do not handle extensions for relative states yet.

	// Create a document and write the header information
	auto doc = bsoncxx::builder::basic::document{};

	doc.append(bsoncxx::builder::basic::kvp("SchemaType", "ModelState"));
	doc.append(bsoncxx::builder::basic::kvp("SchemaVersion_ModelState", (int)1));
	doc.append(bsoncxx::builder::basic::kvp("Version", m_graphCfg.getActiveVersionName()));
	doc.append(bsoncxx::builder::basic::kvp("ParentVersion", getPreviousVersion(m_graphCfg.getActiveVersionName())));
	doc.append(bsoncxx::builder::basic::kvp("Type", "relative"));
	doc.append(bsoncxx::builder::basic::kvp("Extension", extension));
	doc.append(bsoncxx::builder::basic::kvp("BaseState", m_currentModelBaseStateVersion));
	doc.append(bsoncxx::builder::basic::kvp("Description", saveComment));

	// Store the added or modified entities (if any)
	if (!m_addedOrModifiedEntities.empty())
	{
		// Store the topology entities as arrays in the document (id, parent, version)
		auto topo_id = bsoncxx::builder::basic::array();
		auto topo_parent = bsoncxx::builder::basic::array();
		auto topo_version = bsoncxx::builder::basic::array();

		auto data_id = bsoncxx::builder::basic::array();
		auto data_parent = bsoncxx::builder::basic::array();
		auto data_version = bsoncxx::builder::basic::array();

		bool hasTopoEntities = false;
		bool hasDataEntities = false;

		for (auto entity : m_addedOrModifiedEntities)
		{
			switch (entity.second.getEntityType())
			{
			case ModelStateEntity::tEntityType::TOPOLOGY:
				topo_id.append((long long)entity.first);
				topo_parent.append((long long)entity.second.getParentEntityID());
				topo_version.append((long long)entity.second.getEntityVersion());
				hasTopoEntities = true;
				break;
			case ModelStateEntity::tEntityType::DATA:
				data_id.append((long long)entity.first);
				data_parent.append((long long)entity.second.getParentEntityID());
				data_version.append((long long)entity.second.getEntityVersion());
				hasDataEntities = true;
				break;
			default:
				// Unknown type
				assert(0);
			}
		}

		if (hasTopoEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Id", topo_id));
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Parent", topo_parent));
			doc.append(bsoncxx::builder::basic::kvp("topoAddEdit_Version", topo_version));
		}

		if (hasDataEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Id", data_id));
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Parent", data_parent));
			doc.append(bsoncxx::builder::basic::kvp("dataAddEdit_Version", data_version));
		}
	}

	// Store the removed entities (if any)
	if (!m_removedEntities.empty())
	{
		// Store the topology entities as arrays in the document (id, parent, version)
		auto topo_id = bsoncxx::builder::basic::array();
		auto data_id = bsoncxx::builder::basic::array();

		bool hasTopoEntities = false;
		bool hasDataEntities = false;

		for (auto entity : m_removedEntities)
		{
			switch (entity.second.getEntityType())
			{
			case ModelStateEntity::tEntityType::TOPOLOGY:
				topo_id.append((long long)entity.first);
				hasTopoEntities = true;
				break;
			case ModelStateEntity::tEntityType::DATA:
				data_id.append((long long)entity.first);
				hasDataEntities = true;
				break;
			default:
				// Unknown type
				assert(0);
			}
		}

		if (hasTopoEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("topoDel_Id", topo_id));
		}

		if (hasDataEntities)
		{
			doc.append(bsoncxx::builder::basic::kvp("dataDel_Id", data_id));
		}
	}

	// Store the document in the data base
	DataBase::GetDataBase()->StorePlainDataItem(doc);

	return true;
}

std::string ModelState::getCurrentModelStateDescription(void) 
{ 
	return getVersionDescription(m_graphCfg.getActiveVersionName());
}

std::string ModelState::getRedoModelStateDescription(void) 
{ 
	std::string nextVersion = getNextVersion(m_graphCfg.getActiveVersionName());
	if (nextVersion.empty()) return "";

	return getVersionDescription(nextVersion); 
}

bool ModelState::canUndo(void) {
	std::string previousVersion = getPreviousVersion(m_graphCfg.getActiveVersionName());

	return !previousVersion.empty(); 
}

bool ModelState::canRedo(void) 
{ 
	std::string nextVersion = getNextVersion(m_graphCfg.getActiveVersionName());

	return !nextVersion.empty(); 
}

bool ModelState::undoLastOperation(void)
{
	// Determine the parent version
	std::string parentVersion = getPreviousVersion(m_graphCfg.getActiveVersionName());

	if (parentVersion.empty())
	{
		// We are already at the first state
		return false;
	}

	// Load the parent model state
	loadModelState(parentVersion);

	return true;
}

bool ModelState::redoNextOperation(void)
{
	// Determine the next version in the current branch
	std::string nextVersion = getNextVersion(m_graphCfg.getActiveVersionName());

	if (nextVersion.empty())
	{
		// We are already at the last state
		return false;
	}

	// Load the parent model state
	loadModelState(nextVersion);

	return true;
}

void ModelState::removeDanglingModelEntities(void)
{
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	// First of all, we search for the last model state type entry from the back

	auto array_builder = bsoncxx::builder::basic::array{};
	array_builder.append("ModelState");
	array_builder.append("ModelStateExtension");
	array_builder.append("ModelStateInactive");
	array_builder.append("ModelStateExtensionInactive");

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << bsoncxx::builder::stream::open_document
		<< "$in" << array_builder
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	DataStorageAPI::QueryBuilder filter;
	auto filterDoc = filter.GenerateSelectQuery({ "SchemaType" }, true);

	auto sortDoc = bsoncxx::builder::basic::document{};
	sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

	auto result = docBase.GetDocument(std::move(queryDoc), std::move(filterDoc), std::move(sortDoc.extract()));
	if (!result)
	{
		// We did not find a model state, remove the entire content of the project
		assert(0);
		return;
	}

	//std::string propertiesJSON = bsoncxx::to_json(result->view());
	auto lastModelStateID = result->view()["_id"].get_oid();

	// We round down the model state Id such that we surely get all entities younger than this entity, even if they
	// have a potentially smaller oid due to the randomness part in the oid.

	std::string oid = result->view()["_id"].get_oid().value.to_string();
	std::string timePart = oid.substr(0, 8);
	std::string loweroid = timePart + "0000000000000000";
	bsoncxx::oid searchOid(loweroid);

	//std::string test = searchOid.to_string();

	// Now we get the oid of the last model state and get a list of all entries which are older than this state

	queryDoc = bsoncxx::builder::stream::document{}
		<< "_id" << bsoncxx::builder::stream::open_document << "$gte" << searchOid
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	filterDoc = filter.GenerateSelectQuery({ "SchemaType" }, true);

	sortDoc = bsoncxx::builder::basic::document{};
	sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

	auto resultList = docBase.GetAllDocument(std::move(queryDoc), std::move(filterDoc), std::move(sortDoc.extract()), 0);

	// We now take all entries until the first model state and put them into the delete list

	auto deleteEntities = bsoncxx::builder::basic::array();

	if (getListOfNonModelStateEntities(resultList, deleteEntities))
	{
		// If we have entities to delete, we will send the delete call with the list of entities to be deleted to the database server
		auto deleteDoc = bsoncxx::builder::stream::document{}
			<< "_id" << bsoncxx::builder::stream::open_document
			<< "$in" << deleteEntities
			<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

		docBase.DeleteDocuments(std::move(deleteDoc));
	}

	// We should not have dangling entities in the project anymore
}

bool ModelState::getListOfNonModelStateEntities(mongocxx::cursor &cursor, bsoncxx::builder::basic::array &entityArray)
{
	bool entitiesInList = false;

	for (auto item : cursor)
	{
		std::string schemaType = item["SchemaType"].get_utf8().value.data();

		if (schemaType == "ModelState" || schemaType == "ModelStateExtension" || schemaType == "ModelStateInactive" || schemaType == "ModelStateExtensionInactive")
		{
			// We found the fist model state. end the search
			return entitiesInList;
		}
		else
		{
			entityArray.append(item["_id"].get_oid());
			entitiesInList = true;
		}
	}

	return entitiesInList;
}

void ModelState::loadVersionGraph(void) {
	m_graphCfg.clear();

	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto array_builder = bsoncxx::builder::basic::array{};
	array_builder.append("ModelState");
	array_builder.append("ModelStateInactive");

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << bsoncxx::builder::stream::open_document
		<< "$in" << array_builder
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	DataStorageAPI::QueryBuilder filter;
	auto filterDoc = filter.GenerateSelectQuery({ "SchemaType", "Version", "ParentVersion", "Description", "Label" }, false);

	auto results = docBase.GetAllDocument(std::move(queryDoc), std::move(filterDoc), 0);

	for (auto result : results) {
		std::string version = result["Version"].get_utf8().value.data();
		std::string parentVersion = result["ParentVersion"].get_utf8().value.data();

		std::string label;
		std::string description;

		try {
			label = result["Label"].get_utf8().value.data();
		}
		catch (...) {}

		try {
			description = result["Description"].get_utf8().value.data();
		} catch (...) {}
		
		m_graphCfg.insertVersion(version, parentVersion, label, description);
	}
}

ot::VersionGraphCfg& ModelState::getVersionGraph(void) {
	return m_graphCfg;
}

const ot::VersionGraphCfg& ModelState::getVersionGraph(void) const {
	return m_graphCfg;
}

long long ModelState::getCurrentModelEntityVersion(void)
{
	// We search for the last model entity in the database and determine its version
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< bsoncxx::builder::stream::finalize;

	auto emptyFilterDoc = bsoncxx::builder::basic::document{};

	auto sortDoc = bsoncxx::builder::basic::document{};
	sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

	auto result = docBase.GetDocument(std::move(queryDoc), std::move(emptyFilterDoc.extract()), std::move(sortDoc.extract()));
	if (!result) return 0;  // No model entity found

	return result->view()["Version"].get_int64();
}

void ModelState::checkAndUpgradeDataBaseSchema(void)
{
	// Get the schema version of the model entity
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::basic::document{};
	queryDoc.append(bsoncxx::builder::basic::kvp("SchemaType", "Model"));

	auto filterDoc = bsoncxx::builder::basic::document{};

	auto result = docBase.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));
	if (!result) return; // Model state not found

	int schemaVersion = result->view()["SchemaVersion_Model"].get_int64();

	if (schemaVersion == 1)
	{
		// We need to perform a schema upgrade from version 1 to 2.
		updateSchema_1_2();
	}
}

void ModelState::updateSchema_1_2(void)
{
	// Here we need to perform a schema upgrade from version 1 to version 2. This means adding the ParentVersion for all Model states,
	// and adding empty data fields for active branch and active version to the model entity. In addition, we also add a model type
	// Model3D to the model entity

	// First, go through all model states and add their ParentVersions
	// Read the version graph information
	std::list<std::pair<std::string, std::string>> versionGraph;

	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto array_builder = bsoncxx::builder::basic::array{};
	array_builder.append("ModelState");
	array_builder.append("ModelStateInactive");

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << bsoncxx::builder::stream::open_document
		<< "$in" << array_builder
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	DataStorageAPI::QueryBuilder filter;
	auto filterDoc = filter.GenerateSelectQuery({ "SchemaType", "Version" }, false);

	auto results = docBase.GetAllDocument(std::move(queryDoc), std::move(filterDoc), 0);

	std::string parentVersion;
	bool hasInactiveModelState = false;
	for (auto result : results)
	{
		std::string schemaType = result["SchemaType"].get_utf8().value.data();
		std::string version = result["Version"].get_utf8().value.data();

		if (schemaType == "ModelStateInactive") hasInactiveModelState = true;

		versionGraph.push_back(std::pair<std::string, std::string>(version, parentVersion));
		parentVersion = version;
	}

	std::string activeVersion;

	if (hasInactiveModelState)
	{
		// Find the currently active model state
		DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

		queryDoc = bsoncxx::builder::stream::document{}
			<< "SchemaType" << "ModelState"
			<< bsoncxx::builder::stream::finalize;

		auto emptyFilterDoc = bsoncxx::builder::basic::document{};

		auto sortDoc = bsoncxx::builder::basic::document{};
		sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

		auto result = docBase.GetDocument(std::move(queryDoc), std::move(emptyFilterDoc.extract()), std::move(sortDoc.extract()));

		if (result)
		{
			activeVersion = result->view()["Version"].get_utf8().value.data();
		}

		// Turn all inactive model states into active (the current version will be tracked in the model)
		mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("Projects", DataBase::GetDataBase()->getProjectName());

		auto doc_find = bsoncxx::builder::stream::document{}
			<< "SchemaType" << "ModelStateInactive"
			<< bsoncxx::builder::stream::finalize;

		auto doc_modify = bsoncxx::builder::stream::document{}
			<< "$set" << bsoncxx::builder::stream::open_document
			<< "SchemaType" << "ModelState"
			<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

		collection.update_many(doc_find.view(), doc_modify.view());

		doc_find = bsoncxx::builder::stream::document{}
			<< "SchemaType" << "ModelStateExtensionInactive"
			<< bsoncxx::builder::stream::finalize;

		doc_modify = bsoncxx::builder::stream::document{}
			<< "$set" << bsoncxx::builder::stream::open_document
			<< "SchemaType" << "ModelStateExtension"
			<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

		collection.update_many(doc_find.view(), doc_modify.view());
	}

	// Now write the information about the parent version to each model state
	mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("Projects", DataBase::GetDataBase()->getProjectName());

	for (auto item : versionGraph)
	{
		std::string version = item.first;
		std::string parentVersion = item.second;

		// add the parent version to the corresponding entry

		auto array_builder = bsoncxx::builder::basic::array{};
		array_builder.append("ModelState");
		array_builder.append("ModelStateInactive");

		auto queryDoc = bsoncxx::builder::stream::document{}
			<< "SchemaType" << bsoncxx::builder::stream::open_document
			<< "$in" << array_builder
			<< bsoncxx::builder::stream::close_document
			<< "Version" << version 
			<< bsoncxx::builder::stream::finalize;

		auto modifyDoc = bsoncxx::builder::stream::document{}
			<< "$set" << bsoncxx::builder::stream::open_document
			<< "ParentVersion" << parentVersion
			<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

		collection.update_one(queryDoc.view(), modifyDoc.view());
	}

	// Finally, upgrade the model entity
	long long modelVersion = getCurrentModelEntityVersion();

	queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< "Version" << modelVersion
		<< bsoncxx::builder::stream::finalize;

	auto modifyDoc = bsoncxx::builder::stream::document{}
		<< "$set" << bsoncxx::builder::stream::open_document
		<< "SchemaVersion_Model" << (long long) 2
		<< "ModelType" << "Parametric3D" 
		<< "ActiveBranch" << ""
		<< "ActiveVersion" << activeVersion
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	collection.update_one(queryDoc.view(), modifyDoc.view());
}

bool ModelState::isVersionInActiveBranch(const std::string &version)
{
	return isVersionInBranch(version, m_graphCfg.getActiveBranchName());
}

bool ModelState::isVersionInBranch(const std::string &version, const std::string &branch)
{
	assert(!version.empty());

	// We need to count the number of dots in the active branch as well as in the version
	// Comparing them we can ensure that the version is directly in the branch and not in a child branch
	int dotsActive = countNumberOfDots(branch);
	int dotsVersion = countNumberOfDots(version);

	if (branch.empty())
	{
		// This is the main branch. We expect the version to have no dots
		return (dotsVersion == 0);
	}
	else
	{
		// This is a child branch, e.g. 1.1 
		// The versions should then be 1.1.1, 1.1.2, etc.
		if (dotsVersion > dotsActive + 1) return false;

		// Now we need to check whether the part befeore and including the . is the same
		std::string filter = branch + ".";

		// Here we need to check whether the version starts with the given branch + ".". 
		if (version.size() > filter.size())
		{
			// The version could be a version either in the main branch of the given branch or in a child branch
			if (version.substr(0, filter.size()) == filter)
			{
				// This version belongs to the specified branch
				return true;
			}
		}
		else
		{
			// The version may be in a parent branch of this active branch
			// We need to strip off the branch counter after the dot and obtain the version from which 
			// the branch was created.
			int index = branch.rfind('.');
			assert(index != std::string::npos);

			std::string previousVersion = branch.substr(0, index);

			// Now loop through all previous versions and determine whether any of them is the given version
			while (!previousVersion.empty())
			{
				if (previousVersion == version)
				{
					return true; // The version is in the given branch
				}
				previousVersion = getPreviousVersion(previousVersion);
			}
		}
	}

	return false; // The specified version is not in the active branch
}

std::string ModelState::getParentBranch(const std::string &branch)
{
	int index = branch.rfind('.');
	assert(index != std::string::npos);

	return branch.substr(0, index);
}

int ModelState::countNumberOfDots(const std::string& _text)
{
	int count = 0;

	for (size_t index = 0; index < _text.size(); index++)
	{
		if (_text[index] == '.') count++;
	}

	return count;
}

void ModelState::activateBranch(const std::string& _version)
{
	// We need to remove the last . part from the version in order to obtain the branch

	int index = _version.rfind('.');

	if (index == std::string::npos)
	{
		// We are in the main branch
		m_graphCfg.setActiveBranchName("");
	}
	else
	{
		// We are in a child branch
		m_graphCfg.setActiveBranchName(_version.substr(0, index));
	}
}

std::string ModelState::getLastVersionInActiveBranch(void) {
	const ot::VersionGraphVersionCfg* version = m_graphCfg.findLastVersion();
	if (version) {
		return version->getName();
	}
	else {
		return std::string();
	}
}

std::list<std::string> ModelState::removeRedoModelStates(void)
{
	// Find a list of all model State version which follow the current one
	std::list<std::string> futureVersions;
	getAllChildVersions(m_graphCfg.getActiveVersionName(), futureVersions);

	std::list<std::string> removedVersions;

	for (auto version : futureVersions)
	{
		// Now we need to delete the given model state together with all its newly created entities
		deleteModelVersion(version);

		// Remove the version from the version graph
		removeVersionGraphItem(version);

		// And add it to the list of removed versions
		removedVersions.push_back(version);
	}

	// Now it can be that the current branch no longer exists
	// Since we are adding a new version after the current model state, it is best to activate this branch
	activateBranch(m_graphCfg.getActiveVersionName());

	// We need to update the model entity with the new information
	storeCurrentVersionInModelEntity();

	return removedVersions;
}

void ModelState::updateVersionEntity(const std::string& _version) {
	const ot::VersionGraphVersionCfg* version = m_graphCfg.findVersion(_version);
	if (!version) {
		OT_LOG_EAS("Version not found \"" + _version + "\"");
		return;
	}

	mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::basic::document{};
	queryDoc.append(bsoncxx::builder::basic::kvp("SchemaType", "ModelState"));
	queryDoc.append(bsoncxx::builder::basic::kvp("Version", _version));

	auto modifyDoc = bsoncxx::builder::stream::document{}
		<< "$set" << bsoncxx::builder::stream::open_document
		<< "Description" << version->getDescription()
		<< "Label" << version->getLabel()
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	collection.update_one(queryDoc.view(), modifyDoc.view());
}

void ModelState::createAndActivateNewBranch(void)
{
	std::string newBranch;

	int count = 0;
	do
	{
		count++;
		newBranch = m_graphCfg.getActiveVersionName() + "." + std::to_string(count);

	} while (m_graphCfg.getBranchExists(newBranch));

	m_graphCfg.setActiveBranchName(newBranch);
	m_graphCfg.setActiveVersionName(newBranch + ".0"); // This will be incremented during the next save operation
}

void ModelState::storeCurrentVersionInModelEntity(void) {
	if (m_activeVersionInModelEntity != m_graphCfg.getActiveVersionName() || m_activeBranchInModelEntity != m_graphCfg.getActiveBranchName()) {
		if (m_graphCfg.getActiveVersionName().empty()) {
			OT_LOG_E("Attempting to store empty version { \"Version\": \"" + m_graphCfg.getActiveVersionName() + "\", \"Branch\": \"" + m_graphCfg.getActiveBranchName() + "\" }");
			return;
		}
		mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("Projects", DataBase::GetDataBase()->getProjectName());

		// We need to update the model entity
		long long modelVersion = getCurrentModelEntityVersion();

		auto queryDoc = bsoncxx::builder::stream::document{}
			<< "SchemaType" << "Model"
			<< "Version" << modelVersion
			<< bsoncxx::builder::stream::finalize;

		auto modifyDoc = bsoncxx::builder::stream::document{}
			<< "$set" << bsoncxx::builder::stream::open_document
			<< "ActiveBranch" << m_graphCfg.getActiveBranchName()
			<< "ActiveVersion" << m_graphCfg.getActiveVersionName()
			<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

		collection.update_one(queryDoc.view(), modifyDoc.view());

		m_activeBranchInModelEntity = m_graphCfg.getActiveBranchName();
		m_activeVersionInModelEntity = m_graphCfg.getActiveVersionName();
	}
}

std::string ModelState::getPreviousVersion(const std::string& _version) {
	const ot::VersionGraphVersionCfg* version = m_graphCfg.findPreviousVersion(_version);
	if (version) {
		return version->getName();
	}
	else {
		return std::string();
	}
}

std::string ModelState::getVersionDescription(const std::string& _version)
{
	ot::VersionGraphVersionCfg* version = m_graphCfg.findVersion(_version);
	if (version) {
		return version->getDescription();
	}
	else {
		OT_LOG_EAS("Version \"" + _version + "\" does not exist");
		return std::string();
	}
}

void ModelState::removeVersionGraphItem(const std::string& _version) {
	m_graphCfg.removeVersion(_version);
}

bool ModelState::hasNextVersion(const std::string& _version) {
	return !m_graphCfg.versionIsEndOfBranch(_version);
}

std::string ModelState::getNextVersion(const std::string& _version) {
	const ot::VersionGraphVersionCfg* version = m_graphCfg.findNextVersion(_version);
	if (version) {
		return version->getName();
	}
	else {
		return std::string();
	}
}

void ModelState::getAllChildVersions(const std::string& _version, std::list<std::string>& _childVersions) {
	for (const ot::VersionGraphVersionCfg* version : m_graphCfg.findAllNextVersions(_version)) {
		_childVersions.push_back(version->getName());
	}
}

void ModelState::getAllChildVersions(const ot::VersionGraphVersionCfg* _version, std::list<std::string>& _childVersions) {
	OTAssertNullptr(_version);
	this->getAllChildVersions(_version->getName(), _childVersions);
}

void ModelState::deleteModelVersion(const std::string &version) {
	// We only delete the model state information here and leave it up to the garbage collection to remove the 
	// unnecessary entities

	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	// In a first step, get all model states and model state extensions for the to be deleted version
	auto array_builder = bsoncxx::builder::basic::array{};
	array_builder.append("ModelState");
	array_builder.append("ModelStateExtension");

	auto deleteDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << bsoncxx::builder::stream::open_document
		<< "$in" << array_builder
		<< bsoncxx::builder::stream::close_document 
		<< "Version" << version 
		<< bsoncxx::builder::stream::finalize;

	docBase.DeleteDocuments(std::move(deleteDoc));
}
