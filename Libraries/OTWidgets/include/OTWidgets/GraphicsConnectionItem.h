//! @file GraphicsConnectionItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtGui/qpen.h>
#include <QtGui/qpainterpath.h>

namespace ot {
	
	class GraphicsItem;

	class OT_WIDGETS_API_EXPORT GraphicsConnectionItem : public QGraphicsItem {
		OT_DECL_NOCOPY(GraphicsConnectionItem)
	public:
		enum GraphicsConnectionState {
			NoState = 0x00, //! @brief Default state
			HoverState = 0x01, //! @brief Item is hovered over by user
			SelectedState = 0x02  //! @brief Item is selected
		};
		typedef Flags<GraphicsConnectionState> GraphicsConnectionStateFlags;

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

		void updateConnection(void);

		void connectItems(GraphicsItem* _origin, GraphicsItem* _dest);
		void disconnectItems(void);

		GraphicsItem* originItem(void) const { return m_origin; };
		GraphicsItem* destItem(void) const { return m_dest; };

	private:
		void updateConnectionInformation(void);

		void calculatePainterPath(QPainterPath& _path) const;
		void calculateDirectLinePath(QPainterPath& _path) const;
		void calculateSmoothLinePath(QPainterPath& _path) const;
		void calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const;
		void calculateXYLinePath(QPainterPath& _path) const;
		void calculateYXLinePath(QPainterPath& _path) const;
		
		GraphicsConnectionCfg m_config;

		GraphicsConnectionStateFlags m_state;

		GraphicsItem* m_origin;
		GraphicsItem* m_dest;

		QRectF m_lastRect;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsConnectionItem::GraphicsConnectionState)