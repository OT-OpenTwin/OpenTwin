#pragma once
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsItemCfg.h"
#include <memory>

class BlockItemDataProcessing
{
public:
	BlockItemDataProcessing(const std::string& blockName, const std::string& blockTitle, const std::string& blockLabel, const std::string& imageName = "");
	ot::GraphicsItemCfg* GetBlock();
	const std::string& getBlockName()const { return _blockName; }

protected:
	ot::Size2D _blockSize;
	ot::Size2D _connectorSize;
	ot::Color _colourBorder;
	ot::Color _colourTitle;
	ot::Color _colourBackground;

	ot::Color _colourConnector;		

	virtual void AddConnectors(ot::GraphicsFlowItemCfg* block) {};
	virtual void AddBackgroundImage(ot::GraphicsFlowItemCfg* block) {};

private:
	const std::string _blockTitle;
	const std::string _blockLabel;
	const std::string _blockName;
	const std::string _imageName;
};