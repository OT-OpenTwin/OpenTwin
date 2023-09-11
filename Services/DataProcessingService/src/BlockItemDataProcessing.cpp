#include "BlockItemDataProcessing.h"

BlockItemDataProcessing::BlockItemDataProcessing()
{
	_blockSize = ot::Size2D(140, 70);
	_connectorSize = ot::Size2D(12, 12);
	_colourBorder = ot::Color(255, 255, 255);
	_colourConnector = ot::Color(ot::Color::Blue);
	_colourBackground.set(ot::Color::White);
}

ot::GraphicsItemCfg* BlockItemDataProcessing::GetBlock()
{
	ot::GraphicsFlowItemCfg* block = new ot::GraphicsFlowItemCfg;
	
	block->setTitleBackgroundColor(_colourTitle.rInt(), _colourTitle.gInt(), _colourTitle.bInt());
	block->setBackgroundColor(_colourBackground.rInt(), _colourBackground.gInt(), _colourBackground.gInt());

	AddConnectors(block);
	if (_imageName != "")
	{
		block->setBackgroundImagePath("default/" + _imageName);
	}
	
	return block->createGraphicsItem(_blockName,_blockTitle);
}