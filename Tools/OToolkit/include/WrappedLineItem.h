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

	virtual bool rebuildItem(void) override;

	virtual ot::GraphicsItem* getGraphicsItem(void) override { return this; };

	virtual QString getDefaultItemName(void) const override { return "Line"; };

	virtual ot::TreeWidgetItemInfo createNavigationInformation(void) const override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected base class methods

protected:
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(const std::string& _group, const std::string& _item) override;
	virtual void propertyDeleteRequested(const std::string& _group, const std::string& _item) override;

private:

};