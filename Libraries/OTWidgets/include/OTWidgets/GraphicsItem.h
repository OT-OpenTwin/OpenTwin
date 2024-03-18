//! @file GraphicsItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Margins.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/SimpleFactory.h"
#include "OTCore/Flags.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GraphicsConnectionCfg.h"

// Qt header
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicslayoutitem.h>
#include <QtCore/qpoint.h>

// std header
#include <list>
#include <string>

namespace ot {

	class GraphicsScene;
	class GraphicsItemCfg;
	class GraphicsHighlightItem;
	class GraphicsConnectionItem;

	//! @brief Base class for all OpenTwin GraphicsItems
	//! GraphicsItems should be created by the GraphicsFactory and be setup from the corresponding configuration
	class OT_WIDGETS_API_EXPORT GraphicsItem : public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemEvent {
			ItemMoved,
			ItemResized
		};

		enum GraphicsItemState {
			NoState       = 0x00, //! @brief Default state
			HoverState    = 0x01, //! @brief Item is hovered over by user
			SelectedState = 0x02  //! @brief Item is selected
		};

		static QRectF calculateInnerRect(const QRectF& _outerRect, const QSizeF& _innerSize, ot::Alignment _alignment);

		GraphicsItem(bool _isLayoutOrStack);
		virtual ~GraphicsItem();

		// ###############################################################################################################################################

		// Pure virtual functions

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) = 0;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) = 0;

		virtual QGraphicsItem* getQGraphicsItem(void) = 0;

		virtual void prepareGraphicsItemGeometryChange(void) = 0;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const = 0;

		// ###############################################################################################################################################

		// Virtual functions

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg);

		//! @brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) {};

		virtual void graphicsItemFlagsChanged(ot::GraphicsItemCfg::GraphicsItemFlag _flags) {};

		virtual ot::GraphicsItem* findItem(const std::string& _itemName);

		virtual void removeAllConnections(void);

		// ###############################################################################################################################################

		// Event handler

		void handleMousePressEvent(QGraphicsSceneMouseEvent* _event);
		void handleMouseReleaseEvent(QGraphicsSceneMouseEvent* _event);
		void handleHoverEnterEvent(QGraphicsSceneHoverEvent* _event);
		void handleToolTip(QGraphicsSceneHoverEvent* _event);
		void handleHoverLeaveEvent(QGraphicsSceneHoverEvent* _event);
		
		void paintStateBackground(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget);
		
		//! @brief Will expand the size according to the margins
		QSizeF handleGetGraphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _sizeHint) const;

		//! @brief Calculates the actual bounding rect including margins and requested size.
		//! @param _rect The default item rect (the size should be the prefferred item size)
		QRectF handleGetGraphicsItemBoundingRect(const QRectF& _rect) const;

		void handleItemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value);

		void handleSetItemGeometry(const QRectF& _geom);

		// ###############################################################################################################################################

		// Getter / Setter

		void setGraphicsItemFlags(ot::GraphicsItemCfg::GraphicsItemFlag _flags);
		ot::GraphicsItemCfg::GraphicsItemFlag graphicsItemFlags(void) const { return m_flags; };

		void setGraphicsScene(GraphicsScene* _scene) { m_scene = _scene; };
		GraphicsScene* graphicsScene(void);

		void setStateFlags(GraphicsItemState _flags) { m_state = _flags; };
		GraphicsItemState stateFlags(void) const { return m_state; };

		bool isLayoutOrStack(void) const { return m_isLayoutOrStack; };

		void setGraphicsItemUid(const ot::UID& _uid) { m_uid = _uid; };
		const ot::UID& graphicsItemUid(void) const { return m_uid; };

		virtual void setGraphicsItemName(const std::string& _name) { m_name = _name; };
		const std::string& graphicsItemName(void) const { return m_name; };

		virtual void setParentGraphicsItem(GraphicsItem* _itm) { m_parent = _itm; };
		GraphicsItem* parentGraphicsItem(void) const { return m_parent; };
		GraphicsItem* getRootItem(void);

		void storeConnection(GraphicsConnectionItem* _connection);

		//! @brief Removes the collection from the list (item will not be destroyed)
		void forgetConnection(GraphicsConnectionItem* _connection);

		void setGraphicsItemAlignment(ot::Alignment _align) { m_alignment = _align; };
		ot::Alignment graphicsItemAlignment(void) const { return m_alignment; };

		void setConnectionDirection(ot::ConnectionDirection _direction) { m_connectionDirection = _direction; };
		ot::ConnectionDirection connectionDirection(void) const { return m_connectionDirection; };

		void addGraphicsItemEventHandler(ot::GraphicsItem* _handler);
		void removeGraphicsItemEventHandler(ot::GraphicsItem* _handler);

		void raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event);

		QSizeF applyGraphicsItemMargins(const QSizeF& _size) const;
		QSizeF removeGraphicsItemMargins(const QSizeF& _size) const;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size);
		const QSizeF& graphicsItemRequestedSize(void) const { return m_requestedSize; };

		//! @brief Calculates the draw rect for the item
		//! The inner rect takes into account the item geometry, alignment, margins and the actual inner size
		QRectF calculatePaintArea(const QSizeF& _innerSize);

		virtual bool graphicsItemRequiresHover(void) const { return !m_toolTip.empty(); };

		std::list<ot::GraphicsConnectionCfg> getConnectionCfgs();

		void createHighlightItem(void);
		void setHighlightItem(GraphicsHighlightItem* _item);
		GraphicsHighlightItem* highlightItem(void) const { return m_highlightItem; };

	private:
		bool m_isLayoutOrStack;
		
		GraphicsItemState m_state;

		ot::UID m_uid;
		std::string m_name;
		std::string m_toolTip;
		ot::Alignment m_alignment;
		ot::SizePolicy m_sizePolicy;
		ot::MarginsD m_margins;
		GraphicsItemCfg::GraphicsItemFlag m_flags;
		ot::ConnectionDirection m_connectionDirection;
		GraphicsHighlightItem* m_highlightItem;

		QPointF m_moveStartPt; //! @brief Item move origin
		GraphicsItem* m_parent; //! @brief Parent graphics item
		GraphicsScene* m_scene; //! @brief Graphics scene

		QSizeF m_requestedSize;
		QSizeF m_minSize;
		QSizeF m_maxSize;

		std::list<GraphicsItem*> m_eventHandler;
		std::list<GraphicsConnectionItem*> m_connections;

		GraphicsItem() = delete;
		GraphicsItem(const GraphicsItem&) = delete;
		GraphicsItem& operator = (const GraphicsItem&) = delete;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItem::GraphicsItemState)