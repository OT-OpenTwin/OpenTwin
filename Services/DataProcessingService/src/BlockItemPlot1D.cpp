#include "BlockItemPlot1D.h"


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
	return std::shared_ptr<EntityBlock>(new EntityBlock(0,nullptr,nullptr,nullptr,nullptr,""));
}
