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

	void requestPropertyUpdate(ot::UIDList entityIDs, const std::string& propertiesAsJSON);
};
