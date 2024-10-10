//! @file GraphicsConnectionItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/GraphicsElement.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtGui/qpen.h>
#include <QtGui/qpainterpath.h>
#include <QtWidgets/qgraphicsitem.h>

namespace ot {
	
	class GraphicsItem;

	class OT_WIDGETS_API_EXPORT GraphicsConnectionItem : public QGraphicsItem, public GraphicsElement {
		OT_DECL_NOCOPY(GraphicsConnectionItem)
	public:
		GraphicsConnectionItem();
		virtual ~GraphicsConnectionItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsItem

		virtual QRectF boundingRect(void) const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Graphics Base

		//! \brief Calculates and returns the closest distance to the given point.
		//! Returns -1 if the distance is invalid (e.g. maximum distance exceeded).
		//! \param _pt Point in scene coordinates.
		virtual qreal calculateShortestDistanceToPoint(const QPointF& _pt) const override;

		//! \brief Returns the QGraphicsItem.
		virtual QGraphicsItem* getQGraphicsItem(void) override;

		//! \brief Returns the const QGraphicsItem.
		virtual const QGraphicsItem* getQGraphicsItem(void) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		//! @brief Will set the connection properties as provided in the configuration
		//! The origin and destination item won't be set!
		bool setConfiguration(const ot::GraphicsConnectionCfg& _cfg);

		//! @brief Creates a configuration object containing the items origin and destination information
		const GraphicsConnectionCfg& getConfiguration(void) const { return m_config; };

		void setLineShape(GraphicsConnectionCfg::ConnectionShape _shape) { m_config.setLineShape(_shape); this->update(); };
		GraphicsConnectionCfg::ConnectionShape getLineShape(void) const { return m_config.getLineShape(); };

		void setLineWidth(double _width) { m_config.setLineWidth(_width); this->update(); };
		double getLineWidth(void) const { return m_config.getLineWidth(); };

		void setLineColor(const ot::Color& _color) { m_config.setLineColor(_color); this->update(); };

		//! \brief Sets the line painter.
		//! The item takes ownership of the painter.
		void setLinePainter(ot::Painter2D* _painter) { m_config.setLinePainter(_painter); this->update(); };

		//! \brief Returns the current line painter.
		//! The item keeps ownership of the painter.
		const ot::Painter2D* getLinePainter(void) const { return m_config.getLinePainter(); };

		void setLineStyle(const OutlineF& _style) { m_config.setLineStyle(_style); this->update(); };
		const OutlineF& getLineStyle(void) const { return m_config.getLineStyle(); };

		void setHandleState(bool _handlesState);
		bool getHandleState(void) const;

		void updateConnectionView(void);

		//! \brief Sets the origin and destination items.
		//! Will call GraphicsItem::storeConnection for every connected item.
		//! Calls GraphicsConnectionItem::updateConnectionInformation.
		void connectItems(GraphicsItem* _origin, GraphicsItem* _dest);

		//! \brief Unsets the origin and destination items.
		//! Will call GraphicsItem::forgetConnection for every connected item.
		//! Does not call GraphicsConnectionItem::updateConnectionInformation.
		void disconnectItems(void);

		//! \brief Sets the origin and/or destination to null.
		//! If the item is the origin item, origin will be set to null.
		//! If the item is the destination item, destination will be set to null.
		//! Does not call GraphicsConnectionItem::updateConnectionInformation.
		void forgetItem(const GraphicsItem* _item);

		GraphicsItem* originItem(void) const { return m_origin; };
		GraphicsItem* destItem(void) const { return m_dest; };

		//! \brief Updates the configuration according to the origin and/or destination item.
		//! If no orgin is set the origin connectable name will be an empty string and the UID 0.
		//! Same applies to the destination.
		void updateConnectionInformation(void);

	protected:
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _flags) override;

	private:
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

		GraphicsConnectionCfg::ConnectionShape calculateAutoXYShape(void) const;

		ot::Alignment calculateConnectionDirectionAlignment(void) const;

		GraphicsConnectionCfg m_config;

		GraphicsItem* m_origin;
		GraphicsItem* m_dest;

		QRectF m_lastRect;

	};

}
