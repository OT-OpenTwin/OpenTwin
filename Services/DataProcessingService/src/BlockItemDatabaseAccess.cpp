#include "BlockItemDatabaseAccess.h"
#include "EntityBlockDatabaseAccess.h"

BlockItemDatabaseAccess::BlockItemDatabaseAccess(QueryDimension dimension)
	:BlockItemDataProcessing(), _queryDimension(dimension)
{
	_colourTitle.set(ot::Color::Lime);
	_blockName = "Database Access"; 
	_blockTitle = "Database Access";
}


std::shared_ptr<EntityBlock> BlockItemDatabaseAccess::CreateBlockEntity()
{
	std::shared_ptr<EntityBlockDatabaseAccess> dbAccessBlockEntity(new EntityBlockDatabaseAccess(0, nullptr, nullptr, nullptr, nullptr, ""));

	dbAccessBlockEntity->createProperties();

	dbAccessBlockEntity->AddConnector(ot::Connector(ot::ConnectorType::Source, "C0"));
	dbAccessBlockEntity->AddConnector(ot::Connector(ot::ConnectorType::Source, "C1"));
	return dbAccessBlockEntity;
}


void BlockItemDatabaseAccess::AddConnectors(ot::GraphicsFlowItemCfg* block)
{
	block->addRight("C0", "Data output", ot::GraphicsFlowConnectorCfg::Square);
	block->addRight("C1", "Parameter 1", ot::GraphicsFlowConnectorCfg::Square);
	if (_queryDimension != d1)
	{
		block->addRight("C2", "Parameter 2", ot::GraphicsFlowConnectorCfg::Square);
	}
	if (_queryDimension == d3)
	{
		block->addRight("C3", "Parameter 3", ot::GraphicsFlowConnectorCfg::Square);
	}
}
