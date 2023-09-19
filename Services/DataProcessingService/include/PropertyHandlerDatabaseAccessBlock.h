#pragma once
#include "BufferBlockDatabaseAccess.h"
#include "OpenTwinCore/CoreTypes.h"
#include "EntityBlockDatabaseAccess.h"

#include <map>
#include <memory>

class PropertyHandlerDatabaseAccessBlock
{
public:
	bool requiresUpdate(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity);
	OT_rJSON_doc Update(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity,std::list<std::string>& quantityNames, std::list<std::string>& parameterNames, std::list<std::string>& msmdNames);
private:
	std::map<ot::UID, BufferBlockDatabaseAccess> _bufferedInformation;
	const std::string _msmdPropertyName = "Measurement Series";
	const std::string _quantityPropertyName = "Quantity";
	const std::string _groupQuerySpecifications = "Query Specifications";

	void Buffer(std::shared_ptr<EntityBlockDatabaseAccess> dbAccessEntity);
};
