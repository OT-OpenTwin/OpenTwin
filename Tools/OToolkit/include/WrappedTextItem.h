//! @file WrappedTextItem.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"

// OpenTwin header
#include "OTWidgets/GraphicsTextItem.h"

namespace ot { class Painter2D; }

class WrappedTextItem : public ot::GraphicsTextItem, public GraphicsItemDesignerItemBase {
	OT_DECL_NOCOPY(WrappedTextItem)
public:
	WrappedTextItem();
	virtual ~WrappedTextItem();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public base class methods

	virtual bool isDesignedItemCompleted(void) const override { return this->getControlPoints().size() == 1; };

	virtual bool isDesignedItemValid(void) const override { return this->getControlPoints().size() == 1; };

	virtual ot::GraphicsItem* getGraphicsItem(void) override { return this; };

	virtual QString getDefaultItemName(void) const override { return "Text"; };

	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) override;

	virtual void setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	virtual void controlPointsChanged(void) override;
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) override;
};