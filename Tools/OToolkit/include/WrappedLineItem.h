//! @file WrappedLineItem.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"

// OpenTwin header
#include "OTWidgets/GraphicsLineItem.h"

class WrappedLineItem : public ot::GraphicsLineItem, public GraphicsItemDesignerItemBase {
public:
	WrappedLineItem();
	virtual ~WrappedLineItem();

	// ###########################################################################################################################################################################################################################################################################################################################

	virtual bool rebuildItem(void) override;

	virtual ot::GraphicsItem* getGraphicsItem(void) override { return this; };

private:

};