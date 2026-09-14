#include "BlockHandlerRefinement.h"

BlockHandlerRefinement::BlockHandlerRefinement(EntityBlockRefinement* _zoneBlockEntity, const HandlerMap& _handlerMap)
	:BlockHandler(_zoneBlockEntity, _handlerMap)
{
	m_selectedZone = _zoneBlockEntity->getSelectedZone();
}

bool BlockHandlerRefinement::executeSpecialized()
{
	OT_LOG_I("Data-lake access handler executed.");
	return false;
}

std::string BlockHandlerRefinement::getBlockType() const
{
	return "Datalake-Zone Access Block";
}
