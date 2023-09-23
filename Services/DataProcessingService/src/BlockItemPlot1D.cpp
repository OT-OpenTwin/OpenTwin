#include "BlockItemPlot1D.h"
#include "EntityBlockPlot1D.h"
#include "BlockEntityFactoryRegistrar.h"

BlockItemPlot1D::BlockItemPlot1D()
	:BlockItemDataProcessing()
{
	_colourTitle.set(ot::Color::Yellow);
	_blockName = "Plot 1D";
	_blockTitle = "Plot 1D";
}
static BlockEntityFactoryRegistrar factoryEntry("Plot 1D", BlockItemPlot1D::CreateBlockEntity);

void BlockItemPlot1D::AddConnectors(ot::GraphicsFlowItemCfg* block)
{
	block->addLeft("C0", "Y-Axis", ot::GraphicsFlowConnectorCfg::Square);
	block->addLeft("C1", "X-Axis", ot::GraphicsFlowConnectorCfg::Square);
}

std::shared_ptr<EntityBlock> BlockItemPlot1D::CreateBlockEntity()
{
	auto plotBlock = new EntityBlockPlot1D(0, nullptr, nullptr, nullptr, nullptr, "");
	plotBlock->createProperties();
	plotBlock->AddConnector(ot::Connector(ot::ConnectorType::Sink, "C0"));
	plotBlock->AddConnector(ot::Connector(ot::ConnectorType::Sink, "C1"));
	return std::shared_ptr<EntityBlock>(plotBlock);
}
