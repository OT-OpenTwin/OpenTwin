#include "BlockDataProcessing.h"

BlockDataProcessing::BlockDataProcessing(const std::string& blockName, const std::string& blockTitle, const std::string& blockLabel, const std::string& imageName)
	:
	_blockName(blockName), _blockTitle(blockTitle), _blockLabel(blockLabel), _imageName(imageName)
{
	_blockSize = ot::Size2D(140, 70);
	_connectorSize = ot::Size2D(12, 12);
	_colourBorder = ot::Color(255, 255, 255);
	_colourConnector = ot::Color(ot::Color::Blue);
}

ot::GraphicsItemCfg* BlockDataProcessing::GetBlock()
{
	ot::GraphicsStackItemCfg* block = new ot::GraphicsStackItemCfg();
	block->setBottomItem(CreateBaseLayer());
	
	ot::GraphicsStackItemCfg* mainLayerWithConnectors = new ot::GraphicsStackItemCfg();
	mainLayerWithConnectors->setName(_blockName);
	mainLayerWithConnectors->setBottomItem(CreateMainLayer());
	mainLayerWithConnectors->setTopItem(CreateConnectors());
	block->setTopItem(mainLayerWithConnectors);
	return block;
}

ot::GraphicsItemCfg* BlockDataProcessing::CreateBaseLayer()
{
	ot::GraphicsRectangularItemCfg* groundRect = new ot::GraphicsRectangularItemCfg();
	groundRect->setSize(_blockSize);
	groundRect->setBorder(ot::Border(_colourBorder, 2));
	return groundRect;
}

ot::GraphicsItemCfg* BlockDataProcessing::CreateMainLayer()
{
	ot::GraphicsVBoxLayoutItemCfg* centralLayout = new ot::GraphicsVBoxLayoutItemCfg();
	centralLayout->setSize(_blockSize);
	ot::GraphicsHBoxLayoutItemCfg* topLine = new ot::GraphicsHBoxLayoutItemCfg();

	ot::GraphicsTextItemCfg* label = new ot::GraphicsTextItemCfg(_blockLabel, _colourBorder);
	label->setBorder(ot::Border(_colourBorder, 2));
	topLine->addChildItem(label);

	ot::GraphicsTextItemCfg* title = new ot::GraphicsTextItemCfg(_blockTitle, _colourBorder);
	title->setBorder(ot::Border(_colourBorder, 2));
	topLine->addChildItem(title, 1);

	centralLayout->addChildItem(topLine);
	centralLayout->addStrech();
	ot::GraphicsImageItemCfg* symbol = new ot::GraphicsImageItemCfg("default/"+_imageName);
	centralLayout->addChildItem(symbol, 1);

	return centralLayout;
}

ot::GraphicsItemCfg* BlockDataProcessing::CreateConnectors()
{
	ot::GraphicsGridLayoutItemCfg* connectorPositioning = new ot::GraphicsGridLayoutItemCfg(2, 2);
	connectorPositioning->setSize(_blockSize);
	connectorPositioning->setRowStretch(0, 1);
	connectorPositioning->setColumnStretch(0, 1);

	ot::GraphicsRectangularItemCfg* connector = new ot::GraphicsRectangularItemCfg();
	connector->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	connector->setName("C0");
	connector->setSize(_connectorSize);
	connector->setBorder(ot::Border(_colourConnector, 2));
	connectorPositioning->addChildItem(1, 1, connector);

	return connectorPositioning;
}
