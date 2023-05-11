//! @file Block.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include <QtWidgets/qgraphicsitem.h>

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace ot {

	class BlockConnector;

	class BLOCK_EDITOR_API_EXPORT Block : public QObject, public QGraphicsItem {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)

	public:
		Block();
		virtual ~Block();

		virtual QRectF boundingRect(void) const override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget *)nullptr) override;

		virtual qreal blockWidth(void) const = 0;
		virtual qreal blockHeigth(void) const = 0;

		void setHighlighted(bool _isHighlighted = true) { m_isHighlighted = _isHighlighted; };
		bool isHighlighted(void) const { return m_isHighlighted; };

		void setHighlightColor(const QColor& _color) { m_highlightColor = _color; };
		const QColor& highlightColor(void) const { return m_highlightColor; };

	private:
		bool m_isHighlighted;
		QColor m_highlightColor;
	};

}

Q_DECLARE_METATYPE(ot::Block *);