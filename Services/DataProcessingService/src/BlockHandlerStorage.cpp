#include "BlockHandler.h"
#include "BlockHandlerStorage.h"
#include "Application.h"

#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

#include "QuantityDescriptionMatrix.h"
#include "QuantityDescriptionCurve.h"
#include "OTCore/FolderNames.h"

BlockHandlerStorage::BlockHandlerStorage(EntityBlockStorage* blockEntityStorage, const HandlerMap& handlerMap)
	:BlockHandler(blockEntityStorage,handlerMap),m_blockEntityStorage(blockEntityStorage)
{
	m_blockEntityStorage->getInputNames();
}

BlockHandlerStorage::~BlockHandlerStorage()
{
	
}

bool BlockHandlerStorage::executeSpecialized()
{

	if (allInputsAvailable())
	{
		auto& classFactory = Application::instance()->getClassFactory();
		const auto modelComponent = Application::instance()->modelComponent();
		const std::string collectionName = Application::instance()->getCollectionName();
		ResultCollectionExtender resultCollectionExtender(collectionName, *modelComponent, &classFactory, OT_INFO_SERVICE_TYPE_DataProcessingService);

		_uiComponent->displayMessage("Executing Storage Block: " + m_blockName);
		

		resultCollectionExtender.storeCampaignChanges();
		return true;
	}
	else
	{
		return false;
	}
}

bool BlockHandlerStorage::allInputsAvailable()
{
	for (auto& input: m_allInputs)
	{
		if (m_dataPerPort.find(input) == m_dataPerPort.end())
		{
			return false;
		}
	}
	return true;
}

std::list<DatasetDescription> BlockHandlerStorage::createDatasets()
{
	
	std::list<DatasetDescription> allDataDescriptions;

	return allDataDescriptions;
}