/*****************************************************************//**
 * \file   EntityBuffer.h
 * \brief  Meyer' singleton that buffers all entities and their properties that were used so far.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include "CPythonObjectNew.h"
#include "EntityBase.h"
#include <string>
#include <memory>
#include "OTServiceFoundation/ModelServiceAPI.h"
#include "IVisualisationTable.h"
#include "ClassFactory.h"


class EntityBuffer
{
public:
	friend class FixtureEntityBuffer;
	static EntityBuffer& INSTANCE()
	{
		static EntityBuffer instance;
		return instance;
	} 
	void setModelServiceAPI(ot::ModelServiceAPI* _modelServiceAPI) 
	{ 
		assert(_modelServiceAPI != nullptr);
		m_modelServiceAPI = _modelServiceAPI;
	};

	PyObject* getEntityPropertyValue(const std::string& _absoluteEntityName, const std::string& _propertyName);
	PyObject* getTableCellValue(const std::string& _absoluteEntityName, int32_t _row, int32_t _column);
	void updateEntityPropertyValue(const std::string& _absoluteEntityName, const std::string& _propertyName, const CPythonObject& _values);
	std::shared_ptr<EntityBase> getEntity (const std::string& _absoluteEntityName);
	
	void saveChangedEntities();
	bool saveChangedEntities(const std::string& _absoluteEntityName);

	void clearBuffer();

private:
	EntityBuffer() = default;

	std::map<std::string, std::shared_ptr<EntityBase>> m_bufferedEntities;
	std::map<std::string, IVisualisationTable*> m_bufferedTableEntities;
	std::map<std::string, EntityPropertiesBase*> m_bufferedEntityProperties;

	ot::ModelServiceAPI* m_modelServiceAPI = nullptr;
	
	void ensurePropertyToBeLoaded(const std::string& _absoluteEntityName, const std::string& _propertyName);
	void ensureTableToBeLoaded(const std::string& _absoluteEntityName);
	std::shared_ptr<EntityBase> loadEntity(const std::string& _absoluteEntityName);
};
