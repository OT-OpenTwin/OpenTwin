#pragma once
#include "ResultCollectionMetadataAccess.h"
#include "EntityBlockDatabaseAccess.h"
#include "OTServiceFoundation/ModelComponent.h"
#include <map>

class BufferResultCollectionAccess
{
public:
	static BufferResultCollectionAccess& INSTANCE()
	{
		static BufferResultCollectionAccess instance;
		return instance;
	}
	void setModelComponent(ot::components::ModelComponent* modelComponent);
	const std::shared_ptr<ResultCollectionMetadataAccess> getResultCollectionAccessMetadata(EntityBlockDatabaseAccess* blockEntity);

private:
	BufferResultCollectionAccess() {};
	std::map<std::string,std::shared_ptr<ResultCollectionMetadataAccess>> _resultCollectionAccessByProjectName;
	ot::components::ModelComponent* _modelComponent = nullptr;
};
