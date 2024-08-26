#include "BlockHandler.h"
#include "BlockHandlerStorage.h"
#include "Application.h"

BlockHandlerStorage::BlockHandlerStorage(EntityBlockStorage* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity,handlerMap)
{
	auto& classFactory = Application::instance()->getClassFactory();
	const auto modelComponent = Application::instance()->modelComponent();
	const std::string collectionName = Application::instance()->getCollectionName();
	m_resultCollectionExtender = new ResultCollectionExtender(collectionName,*modelComponent,&classFactory,OT_INFO_SERVICE_TYPE_DataProcessingService);

}

BlockHandlerStorage::~BlockHandlerStorage()
{
}

bool BlockHandlerStorage::executeSpecialized()
{
	return false;
}
