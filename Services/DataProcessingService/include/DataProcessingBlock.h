#pragma once
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"


class DataProcessingBlock
{

public:
	DataProcessingBlock();
	std::shared_ptr<ot::GraphicsItemCfg> GetBlock();

protected:
	ot::Size2D _blockSize;
	ot::Color _colourBlack;
	//ot::GraphicsVBoxLayoutItemCfg* _centralLayout;

	virtual void SetTypeSpecificBlockProperties() = 0;
};