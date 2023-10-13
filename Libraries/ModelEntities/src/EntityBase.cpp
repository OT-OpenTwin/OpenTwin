#include "stdafx.h"

#include "EntityBase.h"
#include "DataBase.h"
#include "Types.h"
#include "ClassFactory.h"

_declspec(dllexport) DataStorageAPI::UniqueUIDGenerator *globalUidGenerator = nullptr;

EntityBase::EntityBase(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	entityID(ID),
	entityStorageVersion(0),
	initiallyHidden(false),
	isEditable(false),
	parentEntity(parent),
	observer(obs),
	isModified(true),
	selectChildren(true),
	manageParentVisibility(true),
	manageChildVisibility(true),
	modelState(ms),
	classFactory(factory),
	owningService(owner)
{
}

EntityBase::~EntityBase() 
{ 
	if (parentEntity != nullptr) parentEntity->removeChild(this); 
	if (observer != nullptr) observer->entityRemoved(this);
}

void EntityBase::setUidGenerator(DataStorageAPI::UniqueUIDGenerator *_uidGenerator) 
{ 
	if (globalUidGenerator == nullptr)
	{
		globalUidGenerator = _uidGenerator;
	}
	else
	{
		assert(globalUidGenerator == _uidGenerator);
	}
}

DataStorageAPI::UniqueUIDGenerator *EntityBase::getUidGenerator(void)
{	
	return globalUidGenerator;
}

void EntityBase::setModified(void)
{ 
	if (observer != nullptr) observer->entityModified(this);
	
	isModified = true; 
}

bool EntityBase::updateFromProperties(void)
{
	// This is the standard base class handler. Therefore no specific update code is provided for this entity.
	// We check whether there are any property updates needed (which should not be the case).
	assert(!getProperties().anyPropertyNeedsUpdate());

	return false; // No property grid update necessary
}

void EntityBase::StoreToDataBase(void)
{
	if (!getModified()) return;

	assert(globalUidGenerator != nullptr);
	if (globalUidGenerator == nullptr) return;

	//setEntityStorageVersion(getEntityStorageVersion() + 1);
	entityStorageVersion = globalUidGenerator->getUID();

	entityIsStored();

	// This item collects all information about the entity and adds it to the storage data 
	auto doc = bsoncxx::builder::basic::document{};

	doc.append(bsoncxx::builder::basic::kvp("SchemaType", getClassName()));

	assert(!owningService.empty());

	bsoncxx::document::value bsonObj = bsoncxx::from_json(properties.getJSON(nullptr, false));

	doc.append(bsoncxx::builder::basic::kvp("SchemaVersion_" + getClassName(), getSchemaVersion()),
		bsoncxx::builder::basic::kvp("EntityID", (long long)entityID),
		bsoncxx::builder::basic::kvp("Version", (long long)entityStorageVersion),
		bsoncxx::builder::basic::kvp("Name", name),
		bsoncxx::builder::basic::kvp("initiallyHidden", initiallyHidden),
		bsoncxx::builder::basic::kvp("isEditable", isEditable),
		bsoncxx::builder::basic::kvp("selectChildren", selectChildren),
		bsoncxx::builder::basic::kvp("manageParentVisibility", manageParentVisibility),
		bsoncxx::builder::basic::kvp("manageChildVisibility", manageChildVisibility),
		bsoncxx::builder::basic::kvp("Owner", owningService),
		bsoncxx::builder::basic::kvp("Properties", bsonObj)
	);

	AddStorageData(doc);

	DataBase::GetDataBase()->StoreDataItem(doc);

	resetModified();
}

void EntityBase::StoreToDataBase(ot::UID givenEntityVersion)
{
	if (!getModified()) return;

	entityStorageVersion = givenEntityVersion; 
	entityIsStored();

	// This item collects all information about the entity and adds it to the storage data 
	auto doc = bsoncxx::builder::basic::document{};

	doc.append(bsoncxx::builder::basic::kvp("SchemaType", getClassName()));

	assert(!owningService.empty());

	bsoncxx::document::value bsonObj = bsoncxx::from_json(properties.getJSON(nullptr, false));

	doc.append(bsoncxx::builder::basic::kvp("SchemaVersion_" + getClassName(), getSchemaVersion()),
		bsoncxx::builder::basic::kvp("EntityID", (long long)entityID),
		bsoncxx::builder::basic::kvp("Version", (long long)entityStorageVersion),
		bsoncxx::builder::basic::kvp("Name", name),
		bsoncxx::builder::basic::kvp("initiallyHidden", initiallyHidden),
		bsoncxx::builder::basic::kvp("isEditable", isEditable),
		bsoncxx::builder::basic::kvp("selectChildren", selectChildren),
		bsoncxx::builder::basic::kvp("manageParentVisibility", manageParentVisibility),
		bsoncxx::builder::basic::kvp("manageChildVisibility", manageChildVisibility),
		bsoncxx::builder::basic::kvp("Owner", owningService),
		bsoncxx::builder::basic::kvp("Properties", bsonObj)
	);

	AddStorageData(doc);

	DataBase::GetDataBase()->StoreDataItem(doc);

	resetModified();
}

void EntityBase::restoreFromDataBase(EntityBase *parent, EntityObserver *obs, ModelState *ms, bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	setParent(parent);
	setObserver(obs);
	setModelState(ms);

	readSpecificDataFromDataBase(doc_view, entityMap);

	entityMap[getEntityID()] = this;
}

void EntityBase::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	try
	{
		std::string schemaVersionKey = "SchemaVersion_" + getClassName();
		int schemaVersion = (int) DataBase::GetIntFromView(doc_view, schemaVersionKey.c_str());
		if (schemaVersion != getSchemaVersion()) throw (std::exception());

		entityID             = DataBase::GetIntFromView(doc_view, "EntityID");
		entityStorageVersion = DataBase::GetIntFromView(doc_view, "Version");
		name                 = doc_view["Name"].get_utf8().value.data();
		initiallyHidden      = doc_view["initiallyHidden"].get_bool();
		auto bsonObj         = doc_view["Properties"].get_document();

		owningService = "Model";
		try {
			owningService = doc_view["Owner"].get_utf8().value.data();
		}
		catch (std::exception)
		{
		}

		isEditable = false;
		try {
			isEditable = doc_view["isEditable"].get_bool();
		}
		catch (std::exception)
		{
		}

		selectChildren = true;
		try {
			selectChildren = doc_view["selectChildren"].get_bool();
		}
		catch (std::exception)
		{
		}

		manageParentVisibility = true;
		try {
			manageParentVisibility = doc_view["manageParentVisibility"].get_bool();
		}
		catch (std::exception)
		{
		}

		manageChildVisibility = true;
		try {
			manageChildVisibility = doc_view["manageChildVisibility"].get_bool();
		}
		catch (std::exception)
		{
		}

		std::string propertiesJSON = bsoncxx::to_json(bsonObj);
		properties.buildFromJSON(propertiesJSON);
		properties.forceResetUpdateForAllProperties();
		
		resetModified();
	}
	catch (std::exception)
	{
		assert(0); // read failed
	}
}

void EntityBase::addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds)
{
	if (getEntityID() > 0)
	{
		prefetchIds.push_back(getEntityID());
	}
}

ot::UID EntityBase::createEntityUID(void)
{
	assert(getUidGenerator() != nullptr);

	return getUidGenerator()->getUID();
}

EntityBase *EntityBase::readEntityFromEntityIDAndVersion(EntityBase *parent, ot::UID entityID, ot::UID version, std::map<ot::UID, EntityBase *> &entityMap, ClassFactoryHandler* factory)
{
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(entityID, version, doc))
	{
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	assert(classFactory != nullptr || factory != nullptr);
	EntityBase *entity = (classFactory != nullptr ? classFactory : factory)->CreateEntity(entityType);

	if (entity == nullptr)
	{
		return nullptr;
	}

	entity->restoreFromDataBase(parent, getObserver(), getModelState(), doc_view, entityMap);

	return entity;
}

EntityBase *EntityBase::readEntityFromEntityID(EntityBase *parent, ot::UID entityID, std::map<ot::UID, EntityBase *> &entityMap)
{
	ot::UID version = modelState->getCurrentEntityVersion(entityID);

	return readEntityFromEntityIDAndVersion(parent, entityID, version, entityMap);
}

ot::UID EntityBase::getCurrentEntityVersion(ot::UID entityID)
{
	assert(modelState != nullptr);
	return modelState->getCurrentEntityVersion(entityID);
}

void EntityBase::entityIsStored(void)
{
	// Determined the parent ID
	ot::UID parentID = 0;

	if (getParent() != nullptr)
	{
		parentID = getParent()->getEntityID();
		assert(parentID != 0);
	}

	assert(getEntityID() != 0);
	assert(getEntityStorageVersion() > 0);

	ModelStateEntity::tEntityType entityType = ModelStateEntity::tEntityType::DATA;

	switch (getEntityType())
	{
	case EntityBase::TOPOLOGY:
		entityType = ModelStateEntity::tEntityType::TOPOLOGY;
		break;
	case EntityBase::DATA:
		entityType = ModelStateEntity::tEntityType::DATA;
		break;
	default:
		assert(0); // Unknown entity type
	}

	if (modelState != nullptr)
	{
		modelState->storeEntity(getEntityID(), parentID, getEntityStorageVersion(), entityType);
	}
}

void EntityBase::detachFromHierarchy(void)
{
	// Here we detach the entity from the hierarcy which means detaching from the parent.

	if (getParent() != nullptr)
	{
		getParent()->removeChild(this);
		setParent(nullptr);
	}
}
