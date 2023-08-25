#pragma once
#include "BlockItemDataProcessing.h"
class BlockItemPython : public BlockItemDataProcessing
{
public:
	BlockItemPython();
	void AddConnectors(ot::GraphicsFlowItemCfg* block);
};
