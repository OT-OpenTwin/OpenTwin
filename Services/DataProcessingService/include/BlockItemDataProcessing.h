#pragma once
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsFlowItemCfg.h"
#include <memory>

class BlockItemDataProcessing
{
public:
	BlockItemDataProcessing();
	ot::GraphicsItemCfg* GetBlock();
	const std::string& getBlockName()const { return _blockName; }

protected:
	ot::Size2D _blockSize;
	ot::Size2D _connectorSize;
	ot::Color _colourBorder;
	ot::Color _colourTitle;
	ot::Color _colourBackground;
	ot::Color _colourConnector;		

	std::string _blockTitle = "";
	std::string _blockName = "";
	std::string _imageName = "";

	virtual void AddConnectors(ot::GraphicsFlowItemCfg* block) {};
};