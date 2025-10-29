// @otlicense

//! @file WrappedPolygonItem.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"

// OpenTwin header
#include "OTWidgets/GraphicsPolygonItem.h"

class WrappedPolygonItem : public ot::GraphicsPolygonItem, public GraphicsItemDesignerItemBase {
	OT_DECL_NOCOPY(WrappedPolygonItem)
public:
	WrappedPolygonItem();
	virtual ~WrappedPolygonItem();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public base class methods

	virtual bool isDesignedItemCompleted(void) const override { return this->getControlPoints().size() == INT_MAX; };

	virtual bool isDesignedItemValid(void) const override { return this->getControlPoints().size() > 1; };

	virtual ot::GraphicsItem* getGraphicsItem(void) override { return this; };

	virtual QString getDefaultItemName(void) const override { return "Polygon"; };

	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) override;

	virtual void makeItemTransparent(void) override;

	virtual void setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	virtual void controlPointsChanged(void) override;
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(const ot::Property* _property) override;
	virtual void propertyDeleteRequested(const ot::Property* _property) override;
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) override;

private:

};
