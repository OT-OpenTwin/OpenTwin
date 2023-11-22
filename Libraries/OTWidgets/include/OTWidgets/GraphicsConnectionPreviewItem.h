//! @file GraphicsConnectionPreviewItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtCore/qrect.h>

namespace ot {

	class GraphicsItem;

	class GraphicsConnectionPreviewItem : public QGraphicsItem {
		OT_DECL_NOCOPY(GraphicsConnectionPreviewItem)
	public:
		GraphicsConnectionPreviewItem();
		virtual ~GraphicsConnectionPreviewItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsItem

		virtual QRectF boundingRect(void) const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		void setPen(const QPen& _pen);
		const QPen& pen(void) const { return m_pen; };

		void setConnectionStyle(ot::GraphicsConnectionCfg::ConnectionStyle _style);
		ot::GraphicsConnectionCfg::ConnectionStyle connectionStyle(void) const { return m_style; };

		void setOriginPos(const QPointF& _origin);
		QPointF originPos(void) const { return m_origin; };

		void setOriginDir(ot::ConnectionDirection _direction);
		ot::ConnectionDirection originDir(void) const { return m_originDir; };

		void setDestPos(const QPointF& _dest);
		QPointF destPos(void) const { return m_dest; };

		void setDestDir(ot::ConnectionDirection _direction);
		ot::ConnectionDirection destDir(void) const { return m_originDir; };

	private:
		void calculateSmoothLinePoints(QPointF& _control1, QPointF& _control2) const;
		void calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const;

		QPointF m_origin;
		ot::ConnectionDirection m_originDir;
		QPointF m_dest;
		ot::ConnectionDirection m_destDir;

		QRectF m_lastRect;

		ot::GraphicsConnectionCfg::ConnectionStyle m_style;
		QPen m_pen;
	};

}