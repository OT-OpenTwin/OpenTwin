// @otlicense
// File: GraphicsLayoutItem.h
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

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

class QGraphicsLayout;

namespace ot {

	class GraphicsLayoutItemWrapper;

	class OT_WIDGETS_API_EXPORT GraphicsLayoutItem : public ot::GraphicsItem {
		OT_DECL_NODEFAULT(GraphicsLayoutItem)
		OT_DECL_NOCOPY(GraphicsLayoutItem)
		OT_DECL_NOMOVE(GraphicsLayoutItem)
	public:
		GraphicsLayoutItem(GraphicsItemCfg* _configuration);
		virtual ~GraphicsLayoutItem();
		
		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual void setGraphicsItemName(const std::string& _name) override;

		virtual void removeAllConnections(void) override;

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void setParentGraphicsItem(GraphicsItem* _itm) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const = 0;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(const GraphicsItemCfg::GraphicsItemFlags& _flags) override;

		virtual void graphicsItemConfigurationChanged(const GraphicsItemCfg* _config) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override;
		virtual QGraphicsItem* getQGraphicsItem(void) override;
		virtual const QGraphicsItem* getQGraphicsItem(void) const override;

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

		virtual void finalizeGraphicsItem(void) override;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size) override;

		//! @brief Returns the maximum trigger distance in any direction of this item and its childs.
		virtual double getMaxAdditionalTriggerDistance(void) const override;

		virtual std::list<GraphicsConnectionItem*> getAllConnections() const override;

		virtual std::list<GraphicsElement*> getAllGraphicsElements(void) override;

		virtual std::list<GraphicsElement*> getAllDirectChildElements(void) override;

		GraphicsLayoutItemWrapper* getLayoutWrapper(void) const { return m_layoutWrap; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected

	protected:
		virtual void graphicsSceneSet(GraphicsScene* _scene) override;

		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _state) override;

		virtual void notifyChildsAboutTransformChange(const QTransform& _newTransform) override;

		//! @brief Call this function from the item constructor to create the layout wrapper instance
		void createLayoutWrapper(QGraphicsLayout* _layout);

	private:
		GraphicsLayoutItemWrapper* m_layoutWrap;
	};

}