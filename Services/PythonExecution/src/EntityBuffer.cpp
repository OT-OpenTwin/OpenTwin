#include "EntityBuffer.h"
#include "Application.h"

#include "PropertyPythonObjectConverter.h"
#include "PythonObjectBuilder.h"


PyObject* EntityBuffer::GetEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName)
{
	EnsurePropertyToBeLoaded(absoluteEntityName, propertyName);
	PropertyPythonObjectConverter interface(_bufferedEntityProperties[absoluteEntityName + propertyName]);
	return interface.GetValue();
}

PyObject* EntityBuffer::GetTableCellValue(const std::string& absoluteEntityName, int32_t row, int32_t column)
{
	EnsureTableToBeLoaded(absoluteEntityName);
	std::string cellValue = _bufferedTableEntities[absoluteEntityName]->getTableData()->getValue(row, column);
	PythonObjectBuilder builder;
	auto pCellValue = builder.setString(cellValue);
	Py_INCREF(pCellValue);
	return pCellValue;
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
	_modelServiceAPI->addEntitiesToModel(topoEntID, topoEntVersion, forceVis, dataEnt, dataEnt, dataEnt, "Entity property update by python execution service");
	_modelServiceAPI->updatePropertyGrid();
	ClearBuffer();
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
		_modelServiceAPI->addEntitiesToModel(topoEntID, topoEntVersion, forceVis, dataEnt, dataEnt, dataEnt, "Entity property update by python execution service");
		_bufferedEntities.erase(absoluteEntityName);
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

void EntityBuffer::EnsureTableToBeLoaded(const std::string& absoluteEntityName)
{
	if (_bufferedTableEntities.find(absoluteEntityName) == _bufferedTableEntities.end())
	{
		std::shared_ptr<EntityBase> entity = LoadEntity(absoluteEntityName);
		std::shared_ptr<EntityResultTable<std::string>> newResultTable(dynamic_cast<EntityResultTable<std::string>*>(entity.get()));
		if (newResultTable == nullptr)
		{
			throw std::exception(("Requested table " + absoluteEntityName + " is not a table.").c_str());
		}
		_bufferedTableEntities[absoluteEntityName] = newResultTable;
	}
}


EntityBuffer::EntityBuffer()
{

}

std::shared_ptr<EntityBase> EntityBuffer::LoadEntity(const std::string& absoluteEntityName)
{
	if (_bufferedEntities.find(absoluteEntityName) == _bufferedEntities.end())
	{
		ot::EntityInformation entityInfo;

		_modelServiceAPI->getEntityInformation(absoluteEntityName, entityInfo);
		if (entityInfo.getEntityName() == "")
		{
			throw std::exception(("Requested entity " + absoluteEntityName + " does not exist.").c_str());
		}
		EntityBase* entity = _modelServiceAPI->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
		_bufferedEntities[absoluteEntityName] = std::shared_ptr<EntityBase>(entity);
	}
	return _bufferedEntities[absoluteEntityName];
}

void EntityBuffer::ClearBuffer()
{
	_bufferedEntities.clear();
	_bufferedEntityProperties.clear();
}
