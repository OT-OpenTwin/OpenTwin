// @otlicense
// File: EntityBuffer.h
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

#pragma once
#include "CPythonObjectNew.h"
#include "EntityBase.h"
#include <string>
#include <memory>
#include "IVisualisationTable.h"

//! @brief Singleton that buffers all entities and their properties that were used so far.
class EntityBuffer
{
public:
	friend class FixtureEntityBuffer;
	static EntityBuffer& instance();

	PyObject* getEntityPropertyValue(const std::string& _absoluteEntityName, const std::string& _propertyName);
	PyObject* getTableCellValue(const std::string& _absoluteEntityName, uint32_t _row, uint32_t _column);
	void updateEntityPropertyValue(const std::string& _absoluteEntityName, const std::string& _propertyName, const CPythonObject& _values);
	std::shared_ptr<EntityBase> getEntity(const std::string& _absoluteEntityName);
	
	void saveChangedEntities();
	bool saveChangedEntities(const std::string& _absoluteEntityName);

	void clearBuffer();

private:
	EntityBuffer();

	std::map<std::string, std::shared_ptr<EntityBase>> m_bufferedEntities;
	std::map<std::string, IVisualisationTable*> m_bufferedTableEntities;
	std::map<std::string, EntityPropertiesBase*> m_bufferedEntityProperties;
		
	void ensurePropertyToBeLoaded(const std::string& _absoluteEntityName, const std::string& _propertyName);
	void ensureTableToBeLoaded(const std::string& _absoluteEntityName);
	void ensureValidRangeSelections(EntityBase* _entityBase);

	std::shared_ptr<EntityBase> loadEntity(const std::string& _absoluteEntityName);
};
