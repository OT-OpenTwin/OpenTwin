#include "DataProcessingBlock.h"

DataProcessingBlock::DataProcessingBlock()
	:_blockSize(100, 60), _colourBlack(255, 255, 255)
{
}

std::shared_ptr<ot::GraphicsItemCfg> DataProcessingBlock::GetBlock()
{
	return nullptr;
}
