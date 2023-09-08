#include "BlockItemDatabaseAccess.h"
#include "EntityBlockDatabaseAccess.h"

BlockItemDatabaseAccess::BlockItemDatabaseAccess()
	:BlockItemDataProcessing("Database access", "Database Access", "C", /*"BlockItemDatabaseAccess"*/"")
{
	_colourTitle.set(ot::Color::Lime);
}

std::shared_ptr<EntityBlock> BlockItemDatabaseAccess::CreateBlockEntity()
{
	std::shared_ptr<EntityBlockDatabaseAccess> dbAccessBlockEntity(new EntityBlockDatabaseAccess(0, nullptr, nullptr, nullptr, nullptr, ""));


	dbAccessBlockEntity->createProperties();


	dbAccessBlockEntity->AddConnector(ot::Connector(ot::ConnectorType::Source, "C0"));
	return dbAccessBlockEntity;
}


void BlockItemDatabaseAccess::AddConnectors(ot::GraphicsFlowItemCfg* block)
{
	block->addOutput("C0", "Data output", ot::GraphicsFlowConnectorCfg::Square);

}
