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

	// Public base class methods

	virtual bool isDesignedItemCompleted(void) const override { return this->getControlPoints().size() == 2; };

	virtual ot::GraphicsItem* getGraphicsItem(void) override { return this; };

	virtual QString getDefaultItemName(void) const override { return "Line"; };

	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	virtual void controlPointsChanged(void) override;
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;

private:

};