//! @file BlockNetwork.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockNetwork.h"
#include "OTBlockEditor/Block.h"
#include "OTBlockEditor/BlockConnection.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::BlockNetwork::BlockNetwork() : m_gridSize(10), m_selectedConnection(nullptr) {

}

ot::BlockNetwork::~BlockNetwork() {
	for (auto b : m_blocks) delete b;
	for (auto c : m_connections) delete c;
}

// ########################################################################################################

void ot::BlockNetwork::addBlock(Block* _block) {
	QPoint pos;
	//todo: calculate next free spot as close to the center as possible

	addBlock(_block, pos);
}

void ot::BlockNetwork::addBlock(Block* _block, const QPoint& _pos) {
	_block->moveBy(_pos.x(), _pos.y());
	m_blocks.append(_block);
	addItem(_block);
}

void ot::BlockNetwork::registerConnection(BlockConnection* _connection) {
	m_connections.append(_connection);
	addItem(_connection);
}

// ########################################################################################################

void ot::BlockNetwork::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) {
	QList<QGraphicsItem*> selectedItems = items(_event->scenePos());

	if (m_selectedConnection) {
		m_selectedConnection->setIsUserSelected(false);
		m_selectedConnection = nullptr;
	}

	if (selectedItems.empty())
	{
		update();
		emit backgroundDoubleClicked();
	}
	else
	{
		Block* block = dynamic_cast<Block*>(selectedItems.last());
		BlockConnection* connection = dynamic_cast<BlockConnection*>(selectedItems.last());
		if (block) {
			emit itemDoubleClicked(block);
		}
		else if (connection) {
			m_selectedConnection = connection;
			m_selectedConnection->setIsUserSelected(true);
			update();
		}
		else {
			//todo: replace with otAssert
		}
	}
}

void ot::BlockNetwork::drawBackground(QPainter* _painter, const QRectF& _rect)
{
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
