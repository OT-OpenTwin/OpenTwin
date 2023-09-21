#include "BlockItemDatabaseAccess.h"
#include "EntityBlockDatabaseAccess.h"
#include "BlockEntityFactoryRegistrar.h"

BlockItemDatabaseAccess::BlockItemDatabaseAccess()
	:BlockItemDataProcessing()
{
	_colourTitle.set(ot::Color::Lime);
	_blockName = "Database Access"; 
	_blockTitle = "Database Access";
}
static BlockEntityFactoryRegistrar factoryEntry("Database Access", BlockItemDatabaseAccess::CreateBlockEntity);

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
	block->addOutput("C0", "Data output", ot::GraphicsFlowConnectorCfg::Square);
	block->addOutput("C1", "Parameter 1", ot::GraphicsFlowConnectorCfg::Square);
}
