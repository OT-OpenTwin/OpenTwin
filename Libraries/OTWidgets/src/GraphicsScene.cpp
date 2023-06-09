//! @file GraphicsScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"

// Qt header
#include <QtGui/qpainter.h>

ot::GraphicsScene::GraphicsScene() : m_gridSize(10) {}

ot::GraphicsScene::~GraphicsScene() {}

void ot::GraphicsScene::drawBackground(QPainter* _painter, const QRectF& _rect)
{
	if (m_gridSize < 1) return QGraphicsScene::drawBackground(_painter, _rect);

	QPen pen;
	pen.setColor(QColor(0, 0, 255));
	_painter->setPen(pen);

	qreal left = (qreal)(int(_rect.left()) - (int(_rect.left()) % m_gridSize));
	qreal top = (qreal)(int(_rect.top()) - (int(_rect.top()) % m_gridSize));
	QVector<QPointF> points;
	for (qreal x = left; x < _rect.right(); x += m_gridSize) {
		for (qreal y = top; y < _rect.bottom(); y += m_gridSize) {
			points.append(QPointF(x, y));
		}
	}
	_painter->drawPoints(points.data(), points.size());
}