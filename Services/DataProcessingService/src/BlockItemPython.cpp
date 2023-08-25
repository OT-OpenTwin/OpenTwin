#include "BlockItemPython.h"
#pragma once

BlockItemPython::BlockItemPython()
	:BlockItemDataProcessing("Python","Python","C","")
{
	_colourTitle.set(ot::Color::Cyan);
}

void BlockItemPython::AddConnectors(ot::GraphicsFlowItemCfg* block)
{
	block->addInput("C0", "Parameter", ot::GraphicsFlowConnectorCfg::Square);
	block->addOutput("C0", "Output", ot::GraphicsFlowConnectorCfg::Square);
}
