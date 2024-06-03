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
		OT_DECL_NODEFAULT(GraphicsItem)
		OT_DECL_NOCOPY(GraphicsItem)
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
			NoState           = 0x00, //! @brief Default state
			HoverState        = 0x01, //! @brief Item is hovered over by user
			SelectedState     = 0x02, //! @brief Item is selected
			ForwardSizeState  = 0x08  //! \brief Item forwards requested size requests to child item (e.g. GraphicsLayoutItem).
		};
		//! \typedef GraphicsItemStateFlags
		//! \brief Flags used to manage GraphicsItemState.
		//! \see GraphicsItem, GraphicsItemState
		typedef Flags<GraphicsItemState> GraphicsItemStateFlags;

		//! \brief Constructor.
		//! \param _configuration Initial configuration
		//! \param _stateFlags Initial state flags.
		GraphicsItem(GraphicsItemCfg* _configuration, const ot::Flags<GraphicsItemState>& _stateFlags = ot::Flags<GraphicsItemState>((NoState)));

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

		//! \brief Will setup the item from the provided configuration and store a copy.
		//! The previously stored configuration will be destroyed.
		//! Default 0.
		//! \param _cfg GraphicsItem configuration (Copy will be stored).
		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg);

		//! \brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(GraphicsItem* _sender, GraphicsItemEvent _event) {};

		//! \brief Will be called whenever the GraphicsItem flags have changed.
		virtual void graphicsItemFlagsChanged(const GraphicsItemCfg::GraphicsItemFlags& _flags) {};

		//! \brief Will return any child item that matches the _itemName.
		//! \param _itemName The name of the item to find.
		virtual ot::GraphicsItem* findItem(const std::string& _itemName);

		//! \brief Removes all connections to or from this item.
		virtual void removeAllConnections(void);

		virtual bool graphicsItemRequiresHover(void) const;

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

		//! \brief Handles general item updates.
		//! The selected state will be forwarded to the GraphicsHighlightItem.
		//! Position updates will update the connections and call GraphicsItem::graphicsItemEventHandler() with GraphicsItem::ItemMoved
		void handleItemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value);

		void handleSetItemGeometry(const QRectF& _geom);

		void raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event);

		//! @brief Calculates the draw rect for the item
		//! The inner rect takes into account the item geometry, alignment, margins and the actual inner size
		QRectF calculatePaintArea(const QSizeF& _innerSize);

		// ###############################################################################################################################################

		// Getter / Setter

		//! \brief Returns the current configuration.
		const GraphicsItemCfg* const getConfiguration(void) const { return m_config; };

		//! \brief This function will update the position in the configuration and call QGraphicsItem::setPos
		void setGraphicsItemPos(const QPointF& _pos);

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
		const GraphicsItemStateFlags& getStateFlags(void) const { return m_state; };

		//! \brief Set the GraphicsScene this item is placed at.
		void setGraphicsScene(GraphicsScene* _scene) { m_scene = _scene; };

		//! \brief Returns the GraphicsScene this item is placed at.
		GraphicsScene* getGraphicsScene(void) const { return (m_parent ? m_parent->getGraphicsScene() : m_scene); };

		virtual void setParentGraphicsItem(GraphicsItem* _itm) { m_parent = _itm; };
		GraphicsItem* getParentGraphicsItem(void) const { return m_parent; };
		
		GraphicsItem* getRootItem(void);

		//! \brief Sets the provided flag.
		//! \see GraphicsItem, GraphicsItemFlag
		//! \param _flag Flag to set.
		//! \param _active If true will set the flag, otherwise unset it.
		void setGraphicsItemFlag(ot::GraphicsItemCfg::GraphicsItemFlag _flag, bool _active = true);

		//! \brief Replaces the flags with the flags provided.
		//! \param _flags Flags to set.
		void setGraphicsItemFlags(ot::GraphicsItemCfg::GraphicsItemFlags _flags);

		//! \brief Returns the current GraphicsItemFlags set.
		//! \see GraphicsItem, GraphicsItemFlag
		const GraphicsItemCfg::GraphicsItemFlags& getGraphicsItemFlags(void) const;

		//! \brief Sets the GraphicsItem UID.
		//! \param _uid UID to set.
		void setGraphicsItemUid(const ot::UID& _uid);
		const ot::UID& getGraphicsItemUid(void) const;

		virtual void setGraphicsItemName(const std::string& _name);
		const std::string& getGraphicsItemName(void) const;

		void setGraphicsItemToolTip(const std::string& _toolTip);
		const std::string& getGraphicsItemToolTip(void) const;

		void setGraphicsItemMinimumSize(const QSizeF& _size);
		QSizeF getGraphicsItemMinimumSize(void) const;

		void setGraphicsItemMaximumSize(const QSizeF& _size);
		QSizeF getGraphicsItemMaximumSize(void) const;

		void setGraphicsItemSizePolicy(ot::SizePolicy _policy);
		ot::SizePolicy getGraphicsItemSizePolicy(void) const;

		void setGraphicsItemAlignment(ot::Alignment _align);
		ot::Alignment getGraphicsItemAlignment(void) const;

		void setGraphicsItemMargins(const ot::MarginsD& _margins);
		const ot::MarginsD& getGraphicsItemMargins(void) const;

		void setConnectionDirection(ot::ConnectionDirection _direction);
		ot::ConnectionDirection getConnectionDirection(void) const;

		void storeConnection(GraphicsConnectionItem* _connection);

		//! @brief Removes the collection from the list (item will not be destroyed)
		void forgetConnection(GraphicsConnectionItem* _connection);

		void addGraphicsItemEventHandler(ot::GraphicsItem* _handler);
		void removeGraphicsItemEventHandler(ot::GraphicsItem* _handler);

		QSizeF applyGraphicsItemMargins(const QSizeF& _size) const;
		QSizeF removeGraphicsItemMargins(const QSizeF& _size) const;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size);
		const QSizeF& graphicsItemRequestedSize(void) const { return m_requestedSize; };

		std::list<ot::GraphicsConnectionCfg> getConnectionCfgs();

		void createHighlightItem(void);
		void setHighlightItem(GraphicsHighlightItem* _item);
		GraphicsHighlightItem* highlightItem(void) const { return m_highlightItem; };

	protected:
		//! \brief Returns the configuration for the current item.
		//! The configuration may be modified.
		//! The function will cast the current configuration to the type provided.
		//! The method will return 0 if the cast failed.
		template <class T> T* getItemConfiguration(void);

	private:
		GraphicsItemCfg* m_config; //! \brief Configuration used to setup this item. Default 0.
		
		GraphicsItemStateFlags m_state; //! \brief Current item state flags.
		GraphicsHighlightItem* m_highlightItem; //! \brief Highlight item.

		QPointF m_moveStartPt; //! @brief Item move origin.
		GraphicsItem* m_parent; //! @brief Parent graphics item.
		GraphicsScene* m_scene; //! @brief Graphics scene.

		QSizeF m_requestedSize; //! \brief Size requested by parent.

		std::list<GraphicsItem*> m_eventHandler;
		std::list<GraphicsConnectionItem*> m_connections;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItem::GraphicsItemState)

#include "OTWidgets/GraphicsItem.hpp"