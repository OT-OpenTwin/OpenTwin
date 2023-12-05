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
#include "OpenTwinFoundation/ModelComponent.h"
#include "EntityResultTable.h"
#include "ClassFactory.h"
#include "ClassFactoryBlock.h"

class EntityBuffer
{
public:
	friend class FixtureEntityBuffer;
	static EntityBuffer& INSTANCE()
	{
		static EntityBuffer instance;
		return instance;
	} 
	void setModelComponent(ot::components::ModelComponent* modelComponent) { _modelComponent = modelComponent; };

	PyObject* GetEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName);
	PyObject* GetTableCellValue(const std::string& absoluteEntityName, int32_t row, int32_t column);
	void UpdateEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName, const CPythonObject& values);
	void SaveChangedEntities();
	bool SaveChangedEntities(std::string absoluteEntityName);
	std::shared_ptr<EntityBase> GetEntity (const std::string& absoluteEntityName);

private:
	EntityBuffer();

	ClassFactory _classFactory;
	ClassFactoryBlock _classFactoryBlock;
	std::map<std::string, std::shared_ptr<EntityBase>> _bufferedEntities;
	std::map<std::string, std::shared_ptr<EntityResultTable<std::string>>> _bufferedTableEntities;
	std::map<std::string, EntityPropertiesBase*> _bufferedEntityProperties;

	ot::components::ModelComponent* _modelComponent = nullptr;
	void EnsurePropertyToBeLoaded(const std::string& absoluteEntityName, const std::string& propertyName);
	void EnsureTableToBeLoaded(const std::string& absoluteEntityName);
	std::shared_ptr<EntityBase> LoadEntity(const std::string& absoluteEntityName);

	void ClearBuffer();

};
