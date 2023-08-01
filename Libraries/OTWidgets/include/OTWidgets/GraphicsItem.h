//! @file GraphicsItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicslayoutitem.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsItem : public QGraphicsItem, public QGraphicsLayoutItem {
		Q_INTERFACES(QGraphicsItem)
	public:
		GraphicsItem(QGraphicsItem* _parentItem = (QGraphicsItem *)nullptr);
		virtual ~GraphicsItem();

		virtual QRectF boundingRect(void) const override = 0;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override = 0;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override = 0;
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override = 0;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override = 0;
		
		virtual QSizeF sizeHint(Qt::SizeHint _which, const QSizeF& _constraint) const override = 0;

	private:

	};

}
