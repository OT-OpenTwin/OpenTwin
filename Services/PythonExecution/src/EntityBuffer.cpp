#include "EntityBuffer.h"
#include "Application.h"
#include "IVisualisationTable.h"

#include "PropertyPythonObjectConverter.h"
#include "PythonObjectBuilder.h"


PyObject* EntityBuffer::getEntityPropertyValue(const std::string& _absoluteEntityName, const std::string& _propertyName)
{
	ensurePropertyToBeLoaded(_absoluteEntityName, _propertyName);
	PropertyPythonObjectConverter interface(m_bufferedEntityProperties[_absoluteEntityName + _propertyName]);
	return interface.GetValue();
}

PyObject* EntityBuffer::getTableCellValue(const std::string& _absoluteEntityName, int32_t _row, int32_t _column)
{
	ensureTableToBeLoaded(_absoluteEntityName);
	auto tableData = m_bufferedTableEntities[_absoluteEntityName]->getTable(); 
	
	ot::MatrixEntryPointer matrixEntry;
	matrixEntry.m_column = _column;
	matrixEntry.m_row = _row;
	ot::Variable cellValue  = tableData.getValue(matrixEntry);
	assert(cellValue.isConstCharPtr());

	PythonObjectBuilder builder;
	auto pCellValue = builder.setString(cellValue.getConstCharPtr());
	Py_INCREF(pCellValue);
	return pCellValue;
}

void EntityBuffer::updateEntityPropertyValue(const std::string& _absoluteEntityName, const std::string& _propertyName, const CPythonObject& _values)
{
	ensurePropertyToBeLoaded(_absoluteEntityName, _propertyName);
	PropertyPythonObjectConverter interface(m_bufferedEntityProperties[_absoluteEntityName + _propertyName]);
	interface.SetValue(_values);
}

void EntityBuffer::saveChangedEntities()
{
	ot::UIDList topoEntID, topoEntVersion, dataEnt;
	std::list<bool> forceVis;
	for (auto bufferedEntity : m_bufferedEntities)
	{
		auto entity = bufferedEntity.second;
		entity->StoreToDataBase();
		topoEntID.push_back(entity->getEntityID());
		topoEntVersion.push_back(entity->getEntityStorageVersion());
		forceVis.push_back(false);
	}
	m_modelServiceAPI->addEntitiesToModel(topoEntID, topoEntVersion, forceVis, dataEnt, dataEnt, dataEnt, "Entity property update by python execution service");
	m_modelServiceAPI->updatePropertyGrid();
	clearBuffer();
}

bool EntityBuffer::saveChangedEntities(const std::string& _absoluteEntityName)
{
	if (m_bufferedEntities.find(_absoluteEntityName) != m_bufferedEntities.end())
	{
		std::shared_ptr<EntityBase> entity = m_bufferedEntities[_absoluteEntityName];
		entity->StoreToDataBase();
		ot::UIDList topoEntID, topoEntVersion, dataEnt;
		std::list<bool> forceVis;
		topoEntID.push_back(entity->getEntityID());
		topoEntVersion.push_back(entity->getEntityStorageVersion());
		m_modelServiceAPI->addEntitiesToModel(topoEntID, topoEntVersion, forceVis, dataEnt, dataEnt, dataEnt, "Entity property update by python execution service");
		m_bufferedEntities.erase(_absoluteEntityName);
		return true;
	}
	return false;
}

std::shared_ptr<EntityBase> EntityBuffer::getEntity(const std::string& _absoluteEntityName)
{
	return loadEntity(_absoluteEntityName);
}


void EntityBuffer::ensurePropertyToBeLoaded(const std::string& _absoluteEntityName, const std::string& _propertyName)
{
	if (m_bufferedEntityProperties.find(_absoluteEntityName + _propertyName) == m_bufferedEntityProperties.end())
	{
		std::shared_ptr<EntityBase> entity = loadEntity(_absoluteEntityName);
		EntityPropertiesBase* property = entity->getProperties().getProperty(_propertyName);
		if (property == nullptr)
		{
			throw std::exception(("Requested property " + _propertyName + " does not exist in entity " + _absoluteEntityName).c_str());
		}
		m_bufferedEntityProperties[_absoluteEntityName + _propertyName] = property;
	}
}

void EntityBuffer::ensureTableToBeLoaded(const std::string& _absoluteEntityName)
{
	if (m_bufferedTableEntities.find(_absoluteEntityName) == m_bufferedTableEntities.end())
	{
		std::shared_ptr<EntityBase> entity = loadEntity(_absoluteEntityName);
		std::shared_ptr<IVisualisationTable> newResultTable(dynamic_cast<IVisualisationTable*>(entity.get()));
		if (newResultTable == nullptr)
		{
			throw std::exception(("Requested table " + _absoluteEntityName + " is not a table.").c_str());
		}
		m_bufferedTableEntities[_absoluteEntityName] = newResultTable;
	}
}

std::shared_ptr<EntityBase> EntityBuffer::loadEntity(const std::string& _absoluteEntityName)
{
	if (m_bufferedEntities.find(_absoluteEntityName) == m_bufferedEntities.end())
	{
		ot::EntityInformation entityInfo;

		m_modelServiceAPI->getEntityInformation(_absoluteEntityName, entityInfo);
		if (entityInfo.getEntityName() == "")
		{
			throw std::exception(("Requested entity " + _absoluteEntityName + " does not exist.").c_str());
		}
		EntityBase* entity = m_modelServiceAPI->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
		m_bufferedEntities[_absoluteEntityName] = std::shared_ptr<EntityBase>(entity);
	}
	return m_bufferedEntities[_absoluteEntityName];
}

void EntityBuffer::clearBuffer()
{
	m_bufferedEntities.clear();
	m_bufferedEntityProperties.clear();
}
