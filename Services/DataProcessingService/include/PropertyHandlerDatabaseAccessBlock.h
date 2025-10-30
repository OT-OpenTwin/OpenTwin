// @otlicense
// File: PropertyHandlerDatabaseAccessBlock.h
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

#include "OTCore/CoreTypes.h"
#include "EntityBlockDatabaseAccess.h"
#include "MetadataCampaign.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "ResultCollectionMetadataAccess.h"
#include <map>
#include <memory>

class PropertyHandlerDatabaseAccessBlock : public BusinessLogicHandler
{
public:
	PropertyHandlerDatabaseAccessBlock() =default;

	void performEntityUpdateIfRequired(std::shared_ptr<EntityBlockDatabaseAccess> _dbAccessEntity);
	static ResultCollectionMetadataAccess* getResultCollectionMetadataAccess(EntityBlockDatabaseAccess* _dbAccessEntity, std::string& _collectionName);

private:
	using parameterUpdate = void (EntityBlockDatabaseAccess::*)(const std::string& _unit, const std::string& _type, EntityProperties& _properties);
	const std::string m_selectedValueNone = "";


	void updateSelectionIfNecessary(std::list<std::string>& _valuesInProject, EntityPropertiesSelection* _selection, EntityProperties& _properties);
	std::list<std::string> updateQuantityIfNecessary(std::shared_ptr<EntityBlockDatabaseAccess> _dbAccessEntity, ResultCollectionMetadataAccess* _resultAccess, EntityProperties& _properties);
	void updateParameterIfNecessary(const ResultCollectionMetadataAccess& _resultAccess, const ValueCharacteristicProperties& _selectedProperties, EntityProperties& _properties);
	void resetValueCharacteristicLabelsIfNecessary(const ValueCharacteristicProperties& _selectedProperties, EntityProperties& _properties);
	void resetValueCharacteristicLabelUnit(const ValueCharacteristicProperties& _selectedProperties, EntityProperties& _properties);
	void resetValueCharacteristicLabelDataType(const ValueCharacteristicProperties& _selectedProperties, EntityProperties& _properties);
	void updateIfNecessaryValueCharacteristicLabelUnit(const ValueCharacteristicProperties& _selectedProperties, const std::string& _expectedValue, EntityProperties& _properties);
	void updateIfNecessaryValueCharacteristicLabelDataType(const ValueCharacteristicProperties& _selectedProperties, const std::string& _expectedValue, EntityProperties& _properties);

	void requestPropertyUpdate(ot::UIDList entityIDs, const std::string& propertiesAsJSON);
};
