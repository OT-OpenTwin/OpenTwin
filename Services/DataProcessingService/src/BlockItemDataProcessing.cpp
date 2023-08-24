#include "BlockItemDataProcessing.h"

BlockItemDataProcessing::BlockItemDataProcessing(const std::string& blockName, const std::string& blockTitle, const std::string& blockLabel, const std::string& imageName)
	:
	_blockName(blockName), _blockTitle(blockTitle), _blockLabel(blockLabel), _imageName(imageName)
{
	_blockSize = ot::Size2D(140, 70);
	_connectorSize = ot::Size2D(12, 12);
	_colourBorder = ot::Color(255, 255, 255);
	_colourConnector = ot::Color(ot::Color::Blue);
}

ot::GraphicsItemCfg* BlockItemDataProcessing::GetBlock()
{
	ot::GraphicsFlowItemCfg* block = new ot::GraphicsFlowItemCfg;
	
	block->setName(_blockName);
	block->setTitle(_blockName);
	
	block->setTitleBackgroundColor(_colourTitle.rInt(), _colourTitle.gInt(), _colourTitle.bInt());
	block->setBackgroundColor(_colourBackground.rInt(), _colourBackground.gInt(), _colourBackground.gInt());

	AddConnectors(block);
	if (_imageName != "")
	{
		block->setBackgroundImagePath("default/" + _imageName);
	}
	
	return block;
}