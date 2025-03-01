//! @file GraphicsItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/Margins.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/GraphicsElement.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qpoint.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicslayoutitem.h>

// std header
#include <list>
#include <string>

namespace ot {

	class Painter2D;
	class GraphicsItemCfg;
	class GraphicsConnectionItem;

	//! @brief Base class for all OpenTwin GraphicsItems
	//! GraphicsItems should be created by the GraphicsFactory and be setup from the corresponding configuration
	class OT_WIDGETS_API_EXPORT GraphicsItem : public GraphicsElement {
		OT_DECL_NODEFAULT(GraphicsItem)
		OT_DECL_NOCOPY(GraphicsItem)
	public:

		// ###############################################################################################################################################

		// Types

		//! \enum GraphicsItemEvent
		//! \brief The GraphicsItemEvent is used to describe the type of an event that occured.
		enum GraphicsItemEvent {
			ItemMoved,
			ItemResized
		};

		// ###############################################################################################################################################

		// Public Static

		//! \brief Creates a StyleRefPainter2D painter referencing ot::ColorStyleValueEntry::GraphicsItemSelectionBorder.
		//! The caller takes ownership of the painter.
		static Painter2D* createSelectionBorderPainter(void);

		//! \brief Creates a StyleRefPainter2D painter referencing ot::ColorStyleValueEntry::GraphicsItemHoverBorder.
		//! The caller takes ownership of the painter.
		static Painter2D* createHoverBorderPainter(void);

		// ###############################################################################################################################################

		// Constructor / Destructor

		//! \brief Constructor.
		//! \param _configuration Initial configuration
		//! \param _stateFlags Initial state flags.
		GraphicsItem(GraphicsItemCfg* _configuration);

		//! \brief Destructor.
		virtual ~GraphicsItem();

		// ###############################################################################################################################################

		// Pure virtual functions

		//! \brief Calls QGraphicsItem::paint().
		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) = 0;

		//! \brief Returns the QGraphicsLayoutItem.
		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) = 0;

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
		virtual void graphicsItemFlagsChanged(const GraphicsItemCfg::GraphicsItemFlags& _flags);

		//! \brief Will be called whenever the GraphicsItem configuration has changed.
		virtual void graphicsItemConfigurationChanged(const GraphicsItemCfg* _config);

		//! \brief Will return any child item that matches the _itemName.
		//! \param _itemName The name of the item to find.
		virtual ot::GraphicsItem* findItem(const std::string& _itemName);

		//! \brief Removes all connections to or from this item.
		virtual void removeAllConnections(void);

		virtual bool graphicsItemRequiresHover(void) const;

		//! \brief Will be called after setupFromConfig (i.e. if the item is completely created).
		//! Here the item structure is complete and the item may adjust settings that depend on parent items (e.g. Text reference for GraphicsTextItem).
		//! Container items must override this method to forward the call to their child items.
		virtual void finalizeGraphicsItem(void) {};

		// ###############################################################################################################################################

		// Event handler

		void handleMousePressEvent(QGraphicsSceneMouseEvent* _event);

		//! @brief General event handler for mouse move events.
		//! @return Returns true if the event should be processed.
		bool handleMouseMoveEvent(QGraphicsSceneMouseEvent* _event);
		void handleMouseReleaseEvent(QGraphicsSceneMouseEvent* _event);
		void handleHoverEnterEvent(QGraphicsSceneHoverEvent* _event);
		void handleToolTip(QGraphicsSceneHoverEvent* _event);
		void handleHoverLeaveEvent(QGraphicsSceneHoverEvent* _event);
		
		//! @brief Will expand the size according to the margins
		QSizeF handleGetGraphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _sizeHint) const;

		//! @brief Calculates the actual bounding rect including margins and requested size.
		//! @param _rect The default item rect (the size should be the prefferred item size)
		QRectF handleGetGraphicsItemBoundingRect(const QRectF& _rect) const;

		//! \brief Handles general item updates.
		//! The selected state will be forwarded to the GraphicsHighlightItem.
		//! Position updates will update the connections and call GraphicsItem::graphicsItemEventHandler() with GraphicsItem::ItemMoved
		//! @callgraph
		void handleItemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value);

		void handleSetItemGeometry(const QRectF& _geom);

		void raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event);

		//! \brief Calculates and returns the shortest distance to the given point.
		//! Returns -1 if the distance is invalid (e.g. maximum distance exceeded).
		//! \param _pt Point in scene coordinates.
		virtual qreal calculateShortestDistanceToPoint(const QPointF& _pt) const override;
		qreal calculateShortestDistanceToPoint(double _x, double _y) { return this->calculateShortestDistanceToPoint(QPointF(_x, _y)); };

		//! @brief Calculates the draw rect for the item.
		//! The inner rect takes into account the item geometry, alignment, margins and the actual inner size.
		//! @callgraph
		QRectF calculatePaintArea(const QSizeF& _innerSize);
		QRectF calculatePaintArea(double _innerWidth, double _innerHeight) { return this->calculatePaintArea(QSize(_innerWidth, _innerHeight)); };

		// ###############################################################################################################################################

		// Getter / Setter

		virtual GraphicsScene* getGraphicsScene(void) const override;

		GraphicsItem* getRootItem(void);

		const GraphicsItem* getRootItem(void) const;

		//! \brief Replaces the current configuration with the configuration provided.
		//! The item takes ownership of the configuration.
		void setConfiguration(GraphicsItemCfg* _config);

		//! \brief Returns the current configuration.
		GraphicsItemCfg* const getConfiguration(void) { return m_config; };

		//! \brief Returns the current configuration.
		const GraphicsItemCfg* const getConfiguration(void) const { return m_config; };

		//! \brief Calls GraphicsItem::setGraphicsItemPos(const Point2DD&).
		void setGraphicsItemPos(const QPointF& _pos);
		void setGraphicsItemPos(double _x, double _y) { this->setGraphicsItemPos(QPointF(_x, _y)); };

		//! \brief This function will update the position in the configuration and call QGraphicsItem::setPos(const QPointF&).
		//! @callgraph
		void setGraphicsItemPos(const Point2DD& _pos);

		//! \brief Returns the current position set in the configuration.
		const Point2DD& getGraphicsItemPos(void) const;

		virtual void setParentGraphicsItem(GraphicsItem* _itm) { m_parent = _itm; };
		GraphicsItem* getParentGraphicsItem(void) const { return m_parent; };
		
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

		void setAdditionalTriggerDistance(double _left, double _top, double _right, double _bottom) { this->setAdditionalTriggerDistance(MarginsD(_left, _top, _right, _bottom)); };
		void setAdditionalTriggerDistance(double _distance) { this->setAdditionalTriggerDistance(MarginsD(_distance, _distance, _distance, _distance)); };
		void setAdditionalTriggerDistance(const ot::MarginsD& _distance);
		const ot::MarginsD& getAdditionalTriggerDistance(void) const;
		
		//! \brief Returns the maximum trigger distance in any direction of this item and its childs.
		virtual double getMaxAdditionalTriggerDistance(void) const;
		
		void setGraphicsItemMinimumSize(double _width, double _height) { this->setGraphicsItemMinimumSize(QSizeF(_width, _height)); };
		void setGraphicsItemMinimumSize(const QSizeF& _size);
		QSizeF getGraphicsItemMinimumSize(void) const;
	
		void setGraphicsItemMaximumSize(double _width, double _height) { this->setGraphicsItemMaximumSize(QSizeF(_width, _height)); };
		void setGraphicsItemMaximumSize(const QSizeF& _size);
		QSizeF getGraphicsItemMaximumSize(void) const;

		void setGraphicsItemSizePolicy(ot::SizePolicy _policy);
		ot::SizePolicy getGraphicsItemSizePolicy(void) const;

		void setGraphicsItemAlignment(ot::Alignment _align);
		ot::Alignment getGraphicsItemAlignment(void) const;

		void setGraphicsItemMargins(double _left, double _top, double _right, double _bottom) { this->setGraphicsItemMargins(MarginsD(_left, _top, _right, _bottom)); };
		void setGraphicsItemMargins(double _margins) { this->setGraphicsItemMargins(MarginsD(_margins, _margins, _margins, _margins)); };
		void setGraphicsItemMargins(const ot::MarginsD& _margins);
		const ot::MarginsD& getGraphicsItemMargins(void) const;

		void setConnectionDirection(ot::ConnectionDirection _direction);
		ot::ConnectionDirection getConnectionDirection(void) const;

		void setStringMap(const std::map<std::string, std::string>& _map);
		const std::map<std::string, std::string>& getStringMap(void) const;

		void setGraphicsItemTransform(const Transform& _transform);
		const Transform& getGraphicsItemTransform(void) const;

		void setForwardSizeChanges(bool _forward) { m_forwardSizeChanges = _forward; };
		bool getForwardSizeChanges(void) const { return m_forwardSizeChanges; };

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

		void setGraphicsItemSelected(bool _selected);
		bool getGraphicsItemSelected(void) const;

		//! \see getBlockFlagNotifications
		void setBlockFlagNotifications(bool _block) { m_blockFlagNotifications = _block; };

		//! \brief If enabled the item will not call graphicsItemFlagsChanged() when the flags have changed.
		bool getBlockFlagNotifications(void) const { return m_blockFlagNotifications; };

		//! \see getBlockConfigurationNotifications
		void setBlockConfigurationNotifications(bool _block) { m_blockConfigurationNotifications = _block; };
		
		//! \brief If enabled the item will not call graphicsItemConfigurationChanged() when the flags have changed.
		bool getBlockConfigurationNotifications(void) const { return m_blockConfigurationNotifications; };

		//! \brief Sets the current item position as move start point.
		void setCurrentPosAsMoveStart(void);

		//! \brief Notifies the view if the items current position changed relative to the move start point.
		void notifyMoveIfRequired(void);

		void parentItemTransformChanged(const QTransform& _parentTransform);

		//! \brief Returns the bounding rect in scene coordinates which was expanded by the additional trigger distance according to the config.
		//! \see GraphicsItemCfg::setAdditionalTriggerDistance(const MarginsD& _d)
		QRectF getTriggerBoundingRect(void) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected notifier

	protected:
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _state) override;

		//! \brief Returns the configuration for the current item.
		//! The configuration may be modified.
		//! The function will cast the current configuration to the type provided.
		//! The method will return 0 if the cast failed.
		template <class T> T* getItemConfiguration(void);

		//! \brief Returns the configuration for the current item.
		//! The configuration may be modified.
		//! The function will cast the current configuration to the type provided.
		//! The method will return 0 if the cast failed.
		template <class T> const T* getItemConfiguration(void) const;

		virtual void applyGraphicsItemTransform(void);

		virtual void notifyChildsAboutTransformChange(const QTransform& _newTransform) {};

		//! @brief Returns the cennection direction pointing outwards relative to the parent item.
		//! If no parent item is set returns ConnectionDirection::ConnectAny.
		//! @callgraph
		virtual ConnectionDirection calculateOutwardsConnectionDirection(void) const;

	private:
		GraphicsItemCfg* m_config; //! \brief Configuration used to setup this item. Default 0.
		
		QPointF m_moveStartPt; //! @brief Item move origin.
		GraphicsItem* m_parent; //! @brief Parent graphics item.
		
		QSizeF m_requestedSize; //! \brief Size requested by parent.

		bool m_forwardSizeChanges;

		bool m_blockFlagNotifications;
		bool m_blockConfigurationNotifications;
		
		std::list<GraphicsItem*> m_eventHandler;
		std::list<GraphicsConnectionItem*> m_connections;
	};

}

#include "OTWidgets/GraphicsItem.hpp"