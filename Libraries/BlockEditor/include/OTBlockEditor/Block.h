//! @file Block.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditor/BlockEditorTypes.h"
#include "OTBlockEditor/BlockGraphicsObject.h"
#include "OTGui/LengthLimitation.h"

// Qt header
#include <QtCore/qmimedata.h>
#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include <QtGui/qpixmap.h>
#include <QtWidgets/qgraphicsitem.h>

#define OT_BLOCK_MIMETYPE_Configuration "Configuration"

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QGraphicsScene;

namespace ot {
	
	class BlockConnector;

	class BLOCK_EDITOR_API_EXPORT Block : public QObject, public QGraphicsItem, public ot::BlockGraphicsObject {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)
	public:
		Block();
		virtual ~Block();

		virtual QRectF boundingRect(void) const override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget *)nullptr) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		
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

		void setConfiguration(const QByteArray& _config) { m_config = _config; };
		const QByteArray& configuration(void) const { return m_config; };

		QPixmap toPixmap(void);

		void setBlockContextFlags(const BlockContextFlags& _flags) { m_contextFlags = _flags; };
		const BlockContextFlags& blockContextFlags(void) const { return m_contextFlags; };

		void placeOnScene(QGraphicsScene* _scene);

	protected:
		virtual void placeChildsOnScene(QGraphicsScene* _scene) {};

	private:
		BlockContextFlags m_contextFlags;
		bool m_isHighlighted;
		QColor m_highlightColor;
		LengthLimitation m_heightLimit;
		LengthLimitation m_widthLimit;
		QByteArray m_config;
		bool m_isPressed;
		QPointF m_lastPos;

		Block(const Block&) = delete;
		Block& operator = (const Block&) = delete;
	};

}

Q_DECLARE_METATYPE(ot::Block *);