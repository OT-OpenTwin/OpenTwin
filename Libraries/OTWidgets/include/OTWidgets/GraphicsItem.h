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

// Qt header
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicslayoutitem.h>
#include <QtCore/qpoint.h>

// std header
#include <list>

#define OT_GRAPHICSITEM_MIMETYPE_ItemName "GraphicsItem.Name"

namespace ot {

	class GraphicsItemDrag;
	class GraphicsConnectionItem;
	class GraphicsItemCfg;
	class GraphicsScene;

	//! @brief Base class for all OpenTwin GraphicsItems
	//! GraphicsItems should be created by the GraphicsFactory and be setup from the corresponding configuration
	class OT_WIDGETS_API_EXPORT GraphicsItem : public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemEvent {
			ItemMoved,
			ItemResized
		};

		enum GraphicsItemContext {
			NoContext, //! @brief Item is not placed anywhere
			ItemPreviewContext, //! @brief Item is placed in a preview
			ItemNetworkContext  //! @brief Item is placed in a network
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
		void paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget);

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

		void setGraphicsItemContext(GraphicsItem::GraphicsItemContext _context) { m_context = _context; };
		GraphicsItem::GraphicsItemContext graphicsItemContext(void) const { return m_context; };

		void setGraphicsScene(GraphicsScene* _scene) { m_scene = _scene; };
		GraphicsScene* graphicsScene(void);

		void setHasHover(bool _hasHover) { m_hasHover = _hasHover; };
		bool hasHover(void) const { return m_hasHover; };		

		bool isLayoutOrStack(void) const { return m_isLayoutOrStack; };

		void setGraphicsItemUid(const std::string& _uid) { m_uid = _uid; };
		const std::string& graphicsItemUid(void) const { return m_uid; };

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

	private:
		bool m_isLayoutOrStack;
		bool m_hasHover;
		std::string m_uid;
		std::string m_name;
		std::string m_toolTip;
		ot::Alignment m_alignment;
		ot::SizePolicy m_sizePolicy;
		ot::MarginsD m_margins;
		GraphicsItemCfg::GraphicsItemFlag m_flags;
		GraphicsItemContext m_context;
		ot::ConnectionDirection m_connectionDirection;

		QPointF m_moveStartPt; //! @brief Item move origin
		GraphicsItem* m_parent; //! @brief Parent graphics item
		GraphicsItemDrag* m_drag; //! @brief Drag instance
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
