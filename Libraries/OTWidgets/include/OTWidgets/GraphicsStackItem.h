// @otlicense
// File: GraphicsStackItem.h
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

namespace ot {

	//! @class GraphicsStackItem
	//! @brief The GraphicsStackItem is used adjust the size of child items marked as slave occording to the naster item sizes.
	//! The graphics stack item will calculate the child item size whenever a master item chaged its size.
	//! The GraphicsStackItemCfg is used to configure the GraphicsStackItem.
	class OT_WIDGETS_API_EXPORT GraphicsStackItem : public QGraphicsItemGroup, public QGraphicsLayoutItem, public ot::GraphicsItem {
		OT_DECL_NOCOPY(GraphicsStackItem)
		OT_DECL_NOMOVE(GraphicsStackItem)
	public:
		//! @struct GraphicsStackItemEntry
		//! @brief The GraphicsStackItemEntry holds a GraphicsItem aswell as the information if this item is a master or a slave.
		struct GraphicsStackItemEntry {
			ot::GraphicsItem* item; //! @brief Item.
			bool isMaster; //! @brief If true, the item is a master.
			bool isSlave; //! @brief If true, the item is a slave.
		};

		//! @brief Default constructor.
		GraphicsStackItem();

		//! @brief Destructor.
		virtual ~GraphicsStackItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: GraphicsItem

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual void removeAllConnections(void) override;

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		//! @brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) override;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size) override;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };
		virtual const QGraphicsItem* getQGraphicsItem(void) const override { return this; };

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

		virtual void finalizeGraphicsItem(void) override;

		virtual std::list<GraphicsConnectionItem*> getAllConnections() const override;

		virtual std::list<GraphicsElement*> getAllGraphicsElements(void) override;

		virtual std::list<GraphicsElement*> getAllDirectChildElements(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: QGraphicsItem

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF boundingRect(void) const override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: QGraphicsLayoutItem

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual void setGeometry(const QRectF& rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void addItem(ot::GraphicsItem* _item, bool _isMaster, bool _isSlave);

		//! @brief Returns the maximum trigger distance in any direction of this item and its childs.
		virtual double getMaxAdditionalTriggerDistance(void) const override;

		void clear(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected

	protected:
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _state) override;

		virtual void notifyChildsAboutTransformChange(const QTransform& _newTransform) override;

	private:

		//! @brief Will calculate the size of the slave child items.
		//! @return True if the master size has changed and update() is called.
		bool adjustChildItems(void);

		std::list<GraphicsStackItemEntry> m_items;
		QSizeF m_lastCalculatedSize;
	};


}