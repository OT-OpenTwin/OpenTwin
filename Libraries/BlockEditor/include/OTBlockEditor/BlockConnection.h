//! @file BlockConnection.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qobject.h>
#include <QtWidgets/qgraphicsitem.h>

namespace ot {
	class BlockConnection : public QObject, public QGraphicsItem {
	public:
		BlockConnection();
		virtual ~BlockConnection();

		virtual QPointF pointFrom(void) const = 0;
		virtual QPointF pointTo(void) const = 0;

		virtual QRectF boundingRect(void) const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

		void setLineColor(const QColor& _color) { m_lineColor = _color; };
		const QColor& lineColor(void) const { return m_lineColor; };
		
		void setIsUserSelected(bool _isUserSelected) { m_isUserSelected = _isUserSelected; };
		bool isUserSelected(void) const { return m_isUserSelected; };

	private:
		bool	m_isUserSelected;
		QColor	m_lineColor;

	};
}