// @otlicense

#pragma once

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"

// OpenTwin header
#include "OTWidgets/GraphicsArcItem.h"

namespace ot { class Painter2D; }

class WrappedArcItem : public ot::GraphicsArcItem, public GraphicsItemDesignerItemBase {
	OT_DECL_NOCOPY(WrappedArcItem)
public:
	WrappedArcItem();
	virtual ~WrappedArcItem();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public base class methods

	virtual bool isDesignedItemCompleted(void) const override { return this->getControlPoints().size() == 2; };

	virtual bool isDesignedItemValid(void) const override { return this->getControlPoints().size() == 2; };

	virtual ot::GraphicsItem* getGraphicsItem(void) override { return this; };

	virtual QString getDefaultItemName(void) const override { return "Arc"; };

	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) override;

	virtual void setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	virtual void controlPointsChanged(void) override;
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(const ot::Property* _property) override;
	virtual void propertyDeleteRequested(const ot::Property* _property) override;
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) override;
};