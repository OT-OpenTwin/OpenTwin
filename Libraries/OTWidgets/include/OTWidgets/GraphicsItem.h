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
	class OT_WIDGETS_API_EXPORT GraphicsItem {
	public:
		//! \enum GraphicsItemEvent
		//! \brief The GraphicsItemEvent is used to describe the type of an event that occured.
		enum GraphicsItemEvent {
			ItemMoved,
			ItemResized
		};

		//! \enum GraphicsItemState
		//! \brief The GraphicsItemState is used to describe the current state of a GraphicsItem.
		enum GraphicsItemState {
			NoState       = 0x00, //! @brief Default state
			HoverState    = 0x01, //! @brief Item is hovered over by user
			SelectedState = 0x02  //! @brief Item is selected
		};
		//! \typedef GraphicsItemStateFlags
		//! \brief Flags used to manage GraphicsItemState.
		//! \see GraphicsItem, GraphicsItemState
		typedef Flags<GraphicsItemState> GraphicsItemStateFlags; //!

		//! \brief Constructor.
		//! \param _isLayoutOrStack If true, the item is a layout or a stack item.
		GraphicsItem(bool _isLayoutOrStack);

		//! \brief Destructor.
		virtual ~GraphicsItem();

		// ###############################################################################################################################################

		// Pure virtual functions

		//! \brief Calls QGraphicsItem::paint().
		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) = 0;

		//! \brief Returns the QGraphicsLayoutItem.
		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) = 0;

		//! \brief Returns the QGraphicsItem.
		virtual QGraphicsItem* getQGraphicsItem(void) = 0;

		//! \brief Calls QGraphicsLayoutItem::prepareGeometryChange().
		virtual void prepareGraphicsItemGeometryChange(void) = 0;

		//! \brief Calls QGraphicsLayoutItem::sizeHint().
		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const = 0;

		// ###############################################################################################################################################

		// Virtual functions

		//! \brief Will setup the item from the provided configuration.
		//! \param _cfg GraphicsItem configuration.
		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg);

		//! \brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) {};

		//! \brief Will be called whenever the GraphicsItem flags have changed.
		virtual void graphicsItemFlagsChanged(ot::GraphicsItemCfg::GraphicsItemFlags _flags) {};

		//! \brief Will return any child item that matches the _itemName.
		//! \param _itemName The name of the item to find.
		virtual ot::GraphicsItem* findItem(const std::string& _itemName);

		//! \brief Removes all connections to or from this item.
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

		//! \brief Returns the last set configuration
		const GraphicsItemCfg* const getConfiguration(void) const { return m_config; };

		//! \brief Sets the provided flag.
		//! \see GraphicsItem, GraphicsItemFlag
		//! \param _flag Flag to set.
		//! \param _active If true will set the flag, otherwise unset it.
		void setGraphicsItemFlag(ot::GraphicsItemCfg::GraphicsItemFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };

		//! \brief Replaces the flags with the flags provided.
		//! \param _flags Flags to set.
		void setGraphicsItemFlags(ot::GraphicsItemCfg::GraphicsItemFlags _flags);

		//! \brief Returns the current GraphicsItemFlags set.
		//! \see GraphicsItem, GraphicsItemFlag
		ot::GraphicsItemCfg::GraphicsItemFlags graphicsItemFlags(void) const { return m_flags; };

		//! \brief Set the GraphicsScene this item is placed at.
		void setGraphicsScene(GraphicsScene* _scene) { m_scene = _scene; };

		//! \brief Returns the GraphicsScene this item is placed at.
		GraphicsScene* graphicsScene(void);

		//! \brief Sets the provided state flag.
		//! \see GraphicsItem, GraphicsItemState
		//! \param _state The state to set.
		//! \param _active If true the flag will be set, otherwise unset.
		void setStateFlag(GraphicsItemState _state, bool _active = true) { m_state.setFlag(_state, _active); };

		//! \brief Replaces the flags with the flags provided.
		//! \param _flags Flags to set.
		void setStateFlags(GraphicsItemStateFlags _flags) { m_state = _flags; };

		//! \brief Returns the current GraphicsItemStateFlags set.
		//! \see GraphicsItem, GraphicsItemStateFlags
		GraphicsItemStateFlags stateFlags(void) const { return m_state; };

		//! \brief Returns true if the item is a layout or a stack.
		bool isLayoutOrStack(void) const { return m_isLayoutOrStack; };

		//! \brief Sets the GraphicsItem UID.
		//! \param _uid UID to set.
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
		
		GraphicsItemStateFlags m_state;

		ot::UID m_uid;
		std::string m_name;
		std::string m_toolTip;
		ot::Alignment m_alignment;
		ot::SizePolicy m_sizePolicy;
		ot::MarginsD m_margins;
		GraphicsItemCfg::GraphicsItemFlags m_flags;
		ot::ConnectionDirection m_connectionDirection;
		GraphicsHighlightItem* m_highlightItem;

		GraphicsItemCfg* m_config;

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