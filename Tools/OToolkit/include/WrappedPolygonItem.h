// @otlicense
// File: WrappedPolygonItem.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
