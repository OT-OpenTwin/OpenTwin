#pragma once
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"

class BlockDataProcessing
{
public:
	BlockDataProcessing(const std::string& blockName, const std::string& blockTitle, const std::string& blockLabel, const std::string& imageName = "");
	ot::GraphicsItemCfg* GetBlock();
	const std::string& getBlockName()const { return _blockName; }

protected:
	ot::Size2D _blockSize;
	ot::Size2D _connectorSize;
	ot::Color _colourBorder;
	ot::Color _colourConnector;		

	virtual ot::GraphicsItemCfg* CreateBaseLayer();
	virtual ot::GraphicsItemCfg* CreateMainLayer();
	virtual ot::GraphicsItemCfg* CreateConnectors();

private:
	const std::string _blockTitle;
	const std::string _blockLabel;
	const std::string _blockName;
	const std::string _imageName;
};