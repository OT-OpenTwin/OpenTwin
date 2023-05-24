//! @file Block.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditor/BlockGraphicsObject.h"
#include "OTGui/LengthLimitation.h"

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

	class BLOCK_EDITOR_API_EXPORT BlockGraphicsItemGroup : public QGraphicsItemGroup {
	public:
		BlockGraphicsItemGroup() {};
		virtual ~BlockGraphicsItemGroup() {};

	private:

		BlockGraphicsItemGroup(const BlockGraphicsItemGroup&) = delete;
		BlockGraphicsItemGroup& operator = (const BlockGraphicsItemGroup&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class BLOCK_EDITOR_API_EXPORT Block : public QObject, public QGraphicsItem, public ot::BlockGraphicsObject {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)

	public:
		Block(BlockGraphicsItemGroup* _graphicsItemGroup);
		virtual ~Block();

		virtual QRectF boundingRect(void) const override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget *)nullptr) override;

		virtual qreal blockWidth(void) const = 0;
		virtual qreal blockHeigth(void) const = 0;

		void setGraphicsItemGroup(BlockGraphicsItemGroup* _gig) { m_gig = _gig; };
		BlockGraphicsItemGroup* graphicsItemGroup(void) { return m_gig; }

		void setHighlighted(bool _isHighlighted = true) { m_isHighlighted = _isHighlighted; };
		bool isHighlighted(void) const { return m_isHighlighted; };

		void setHighlightColor(const QColor& _color) { m_highlightColor = _color; };
		const QColor& highlightColor(void) const { return m_highlightColor; };

		void setHeightLimit(const LengthLimitation& _limit) { m_heightLimit = _limit; };
		LengthLimitation& getHeightLimit(void) { return m_heightLimit; };
		const LengthLimitation& heightLimit(void) const { return m_heightLimit; };

		void setWidthLimit(const LengthLimitation& _limit) { m_widthLimit = _limit; };
		LengthLimitation& getWidthLimit(void) { return m_widthLimit; };
		const LengthLimitation& widthLimit(void) const { return m_widthLimit; };

	private:
		BlockGraphicsItemGroup* m_gig;
		bool m_isHighlighted;
		QColor m_highlightColor;
		LengthLimitation m_heightLimit;
		LengthLimitation m_widthLimit;

		Block() = delete;
		Block(const Block&) = delete;
		Block& operator = (const Block&) = delete;
	};

}

Q_DECLARE_METATYPE(ot::Block *);