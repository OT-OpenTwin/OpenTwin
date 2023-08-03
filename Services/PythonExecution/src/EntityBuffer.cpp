#include "EntityBuffer.h"
#include "ClassFactory.h"
#include "PropertyPythonObjectConverter.h"

PyObject* EntityBuffer::GetEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName)
{
	EnsurePropertyToBeLoaded(absoluteEntityName, propertyName);
	PropertyPythonObjectConverter interface(_bufferedEntityProperties[absoluteEntityName + propertyName]);
	return interface.GetValue();
}

void EntityBuffer::UpdateEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName, const CPythonObject& values)
{
	EnsurePropertyToBeLoaded(absoluteEntityName, propertyName);
	PropertyPythonObjectConverter interface(_bufferedEntityProperties[absoluteEntityName + propertyName]);
	interface.SetValue(values);
}

void EntityBuffer::SaveChangedEntities()
{
	ot::UIDList topoEntID, topoEntVersion, dataEnt;
	std::list<bool> forceVis;
	for (auto bufferedEntity : _bufferedEntities)
	{
		auto entity = bufferedEntity.second;
		entity->StoreToDataBase();
		topoEntID.push_back(entity->getEntityID());
		topoEntVersion.push_back(entity->getEntityStorageVersion());
		forceVis.push_back(false);
	}
	_modelComponent->addEntitiesToModel(topoEntID, topoEntVersion, forceVis, dataEnt, dataEnt, dataEnt, "Entity property update by python execution service");
}

bool EntityBuffer::SaveChangedEntities(std::string absoluteEntityName)
{
	if (_bufferedEntities.find(absoluteEntityName) != _bufferedEntities.end())
	{
		std::shared_ptr<EntityBase> entity = _bufferedEntities[absoluteEntityName];
		entity->StoreToDataBase();
		ot::UIDList topoEntID, topoEntVersion, dataEnt;
		std::list<bool> forceVis;
		topoEntID.push_back(entity->getEntityID());
		topoEntVersion.push_back(entity->getEntityStorageVersion());
		_modelComponent->addEntitiesToModel(topoEntID, topoEntVersion, forceVis, dataEnt, dataEnt, dataEnt, "Entity property update by python execution service");
		return true;
	}
	return false;
}

std::shared_ptr<EntityBase> EntityBuffer::GetEntity(const std::string& absoluteEntityName)
{
	return LoadEntity(absoluteEntityName);
}


void EntityBuffer::EnsurePropertyToBeLoaded(const std::string& absoluteEntityName, const std::string& propertyName)
{
	if (_bufferedEntityProperties.find(absoluteEntityName + propertyName) == _bufferedEntityProperties.end())
	{
		std::shared_ptr<EntityBase> entity = LoadEntity(absoluteEntityName);
		EntityPropertiesBase* property = entity->getProperties().getProperty(propertyName);
		if (property == nullptr)
		{
			throw std::exception(("Requested property " + propertyName + " does not exist in entity " + absoluteEntityName).c_str());
		}
		_bufferedEntityProperties[absoluteEntityName + propertyName] = property;
	}
}

std::shared_ptr<EntityBase> EntityBuffer::LoadEntity(const std::string& absoluteEntityName)
{
	if (_bufferedEntities.find(absoluteEntityName) == _bufferedEntities.end())
	{
		ot::EntityInformation entityInfo;
		ClassFactory* classFactory = new ClassFactory();
		_modelComponent->getEntityInformation(absoluteEntityName, entityInfo);
		if (entityInfo.getName() == "")
		{
			throw std::exception(("Requested entity " + absoluteEntityName + " does not exist.").c_str());
		}
		EntityBase* entity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), *classFactory);
		_bufferedEntities[absoluteEntityName] = std::shared_ptr<EntityBase>(entity);
	}
	return _bufferedEntities[absoluteEntityName];
}

void EntityBuffer::ClearBuffer()
{
	_bufferedEntities.clear();
	_bufferedEntityProperties.clear();
}
