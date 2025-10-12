//! @file GraphicsConnectionItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/GraphicsElement.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtGui/qpen.h>
#include <QtGui/qpainterpath.h>
#include <QtWidgets/qgraphicsitem.h>

namespace ot {
	
	class GraphicsItem;
	class GraphicsDisconnectItem;
	class GraphicsConnectionConnectorItem;

	class OT_WIDGETS_API_EXPORT GraphicsConnectionItem : public QGraphicsItem, public GraphicsElement {
		OT_DECL_NOCOPY(GraphicsConnectionItem)
	public:
		GraphicsConnectionItem();
		virtual ~GraphicsConnectionItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsItem

		virtual QRectF boundingRect() const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Graphics Base

		//! @brief Calculates and returns the closest distance to the given point.
		//! Returns -1 if the distance is invalid (e.g. maximum distance exceeded).
		//! @param _pt Point in scene coordinates.
		virtual qreal calculateShortestDistanceToPoint(const QPointF& _pt) const override;

		//! @brief Returns the QGraphicsItem.
		virtual QGraphicsItem* getQGraphicsItem() override;

		//! @brief Returns the const QGraphicsItem.
		virtual const QGraphicsItem* getQGraphicsItem() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Configuration

		//! @brief Will set the connection properties as provided in the configuration
		//! The origin and destination item won't be set!
		bool setConfiguration(const ot::GraphicsConnectionCfg& _cfg);

		//! @brief Creates a configuration object containing the items origin and destination information
		const GraphicsConnectionCfg& getConfiguration() const { return m_config; };

		void setLineShape(GraphicsConnectionCfg::ConnectionShape _shape);
		GraphicsConnectionCfg::ConnectionShape getLineShape() const { return m_config.getLineShape(); };

		void setLineWidth(double _width);
		double getLineWidth() const { return m_config.getLineWidth(); };

		void setLineColor(const ot::Color& _color) { m_config.setLineColor(_color); this->update(); };

		//! @brief Sets the line painter.
		//! The item takes ownership of the painter.
		void setLinePainter(ot::Painter2D* _painter) { m_config.setLinePainter(_painter); this->update(); };

		//! @brief Returns the current line painter.
		//! The item keeps ownership of the painter.
		const ot::Painter2D* getLinePainter() const { return m_config.getLinePainter(); };

		void setLineStyle(const PenFCfg& _style);
		const PenFCfg& getLineStyle() const { return m_config.getLineStyle(); };

		void setHandleState(bool _handlesState) { m_config.setHandlesState(_handlesState); };
		bool getHandleState() const { return m_config.getHandlesState(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Item handling

		//! @brief Sets the origin and destination items.
		//! Will call GraphicsItem::storeConnection for every connected item.
		//! Calls GraphicsConnectionItem::updateConnectionInformation.
		void connectItems(GraphicsItem* _origin, GraphicsItem* _dest);

		//! @brief Set the origin item.
		//! Will call GraphicsItem::storeConnection for the connected item.
		//! @param _origin Origin item to connect to.
		void setOriginItem(GraphicsItem* _origin);
		
		//! @brief Set the origin position.
		//! This should only be used if no origin item is set.
		//! @param _pos Position in scene coordinates.
		void setOriginPos(const Point2DD& _pos);

		//! @brief Set the destination item.
		//! Will call GraphicsItem::storeConnection for the connected item.
		//! @param _dest Destination item to connect to.
		void setDestItem(GraphicsItem* _dest);

		//! @brief Set the destination position.
		//! This should only be used if no destination item is set.
		//! @param _pos Position in scene coordinates.
		void setDestPos(const Point2DD& _pos);

		//! @brief Unsets the origin and destination items.
		//! Will call GraphicsItem::forgetConnection for every connected item.
		//! Does not call GraphicsConnectionItem::updateConnectionInformation.
		void disconnectItems(bool _updateConfig = false);

		//! @brief Sets the origin and/or destination to null.
		//! If the item is the origin item, origin will be set to null.
		//! If the item is the destination item, destination will be set to null.
		//! Does not call GraphicsConnectionItem::updateConnectionInformation.
		void disconnectItem(const GraphicsItem* _item, bool _updateConfig = false);

		void disconnectRequested(GraphicsDisconnectItem* _disconnector);

		GraphicsItem* getOriginItem() const { return m_origin; };
		GraphicsItem* getDestItem() const { return m_dest; };

		void updatePositionsFromItems();

		void updateConnectionView();

		//! @brief Updates the configuration according to the origin and/or destination item.
		//! If no orgin is set the origin connectable name will be an empty string and the UID 0.
		//! Same applies to the destination.
		void updateConnectionInformation();

	protected:
		virtual void graphicsSceneSet(GraphicsScene* _scene) override;
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _flags) override;

	private:
		QPointF calculateOriginPos() const;
		QPointF calculateDestPos() const;
		ConnectionDirection calculateOriginDirection() const;
		ConnectionDirection calculateDestDirection() const;

		void updateConnectors();
		void updateDisconnectVisibility();

		void calculatePainterPath(QPainterPath& _path) const;
		void calculateDirectLinePath(QPainterPath& _path) const;
		void calculateSmoothLinePath(QPainterPath& _path) const;
		void calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const;
		void calculateXYLinePath(QPainterPath& _path) const;
		void calculateYXLinePath(QPainterPath& _path) const;
		void calculateAutoXYLinePath(QPainterPath& _path) const;
		qreal calculateShortestDistanceToPointDirect(const QPointF& _pt) const;
		qreal calculateShortestDistanceToPointSmooth(const QPointF& _pt) const;
		qreal calculateShortestDistanceToPointXY(const QPointF& _pt) const;
		qreal calculateShortestDistanceToPointYX(const QPointF& _pt) const;
		qreal calculateShortestDistanceToPointAutoXY(const QPointF& _pt) const;

		GraphicsConnectionCfg::ConnectionShape calculateAutoXYShape() const;

		ot::Alignment calculateConnectionDirectionAlignment() const;

		GraphicsConnectionCfg m_config;

		GraphicsItem* m_origin;
		GraphicsDisconnectItem* m_originDisconnect;
		GraphicsConnectionConnectorItem* m_originConnector;

		GraphicsItem* m_dest;
		GraphicsDisconnectItem* m_destDisconnect;
		GraphicsConnectionConnectorItem* m_destConnector;

		QRectF m_lastRect;
		QPointF m_disconnectOffset;
	};

}
