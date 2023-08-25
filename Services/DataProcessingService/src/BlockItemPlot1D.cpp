#include "BlockItemPlot1D.h"
#include "EntityBlockPlot1D.h"

BlockItemPlot1D::BlockItemPlot1D()
	:BlockItemDataProcessing("Plot 1D", "Plot 1D","V", /*"Plot1DVisible"*/"")
{
	_colourTitle.set(ot::Color::Yellow);
}

void BlockItemPlot1D::AddConnectors(ot::GraphicsFlowItemCfg* block)
{
	block->addInput("C0", "Y-Axis", ot::GraphicsFlowConnectorCfg::Square);
}

std::shared_ptr<EntityBlock> BlockItemPlot1D::CreateBlockEntity()
{
	auto plotBlock = new EntityBlockPlot1D(0, nullptr, nullptr, nullptr, nullptr, "");
	plotBlock->createProperties();
	plotBlock->AddConnector(ot::Connector(ot::ConnectorType::Sink, "C0"));
	return std::shared_ptr<EntityBlock>(plotBlock);
}
