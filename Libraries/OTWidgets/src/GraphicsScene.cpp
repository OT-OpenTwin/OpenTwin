//! @file GraphicsScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/GraphicsConnectionPreviewItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::GraphicsScene::GraphicsScene(GraphicsView* _view)
	: m_gridStepSize(10), m_gridWideEvery(10), m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr),
	m_connectionPreviewStyle(ot::GraphicsConnectionCfg::DirectLine), m_ignoreEvents(false), m_mouseIsPressed(false),
	m_penWidth(1.), m_gridMode(AdvancedGrid)
{
	this->connect(this, &GraphicsScene::selectionChanged, this, &GraphicsScene::slotSelectionChanged);
}

ot::GraphicsScene::GraphicsScene(const QRectF& _sceneRect, GraphicsView* _view)
	: QGraphicsScene(_sceneRect), m_gridStepSize(10), m_gridWideEvery(10), m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr),
	m_connectionPreviewStyle(ot::GraphicsConnectionCfg::DirectLine), m_ignoreEvents(false), m_mouseIsPressed(false),
	m_penWidth(1.), m_gridMode(AdvancedGrid)
{
	this->connect(this, &GraphicsScene::selectionChanged, this, &GraphicsScene::slotSelectionChanged);
}

ot::GraphicsScene::~GraphicsScene() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Connection handling

void ot::GraphicsScene::startConnection(ot::GraphicsItem* _item) {
	if (m_connectionOrigin == nullptr) {
		// Start new connection
		m_connectionOrigin = _item;

		m_connectionPreview = new GraphicsConnectionPreviewItem;
		QPen p;
		p.setColor(QColor(64, 64, 255));
		p.setWidth(1);
		m_connectionPreview->setPen(p);
		m_connectionPreview->setConnectionStyle(m_connectionPreviewStyle);
		m_connectionPreview->setOriginPos(m_connectionOrigin->getQGraphicsItem()->scenePos() + m_connectionOrigin->getQGraphicsItem()->boundingRect().center());
		m_connectionPreview->setOriginDir(m_connectionOrigin->getConnectionDirection());
		m_connectionPreview->setDestPos(m_connectionPreview->originPos());
		m_connectionPreview->setDestDir(ot::inversedConnectionDirection(m_connectionOrigin->getConnectionDirection()));
		this->addItem(m_connectionPreview);

		return;
	}
	else {
		if (m_connectionOrigin == _item) {
			this->stopConnection();
			return;
		}
		OT_LOG_D("New conncetion");
		m_view->requestConnection(m_connectionOrigin->getRootItem()->getGraphicsItemUid(), m_connectionOrigin->getGraphicsItemName(), _item->getRootItem()->getGraphicsItemUid(), _item->getGraphicsItemName());
		this->stopConnection();
	}
}

void ot::GraphicsScene::stopConnection(void) {
	// Stop connection
	if (m_connectionPreview) {
		this->removeItem(m_connectionPreview);
		delete m_connectionPreview;
		m_connectionPreview = nullptr;
		m_connectionOrigin = nullptr;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void ot::GraphicsScene::slotSelectionChanged(void) {
	if (m_ignoreEvents || m_mouseIsPressed) return;
	this->handleSelectionChanged();
}

void ot::GraphicsScene::handleSelectionChanged(void) {
	QList<QGraphicsItem*> tmp = m_lastSelection;
	m_lastSelection = this->selectedItems();
	if (tmp.size() != m_lastSelection.size()) {
		Q_EMIT selectionChangeFinished();
	}
	else {
		for (QGraphicsItem* itm : m_lastSelection) {
			bool found = false;
			for (QGraphicsItem* itmCheck : tmp) {
				if (itm == itmCheck) {
					found = true;
					break;
				}
			}
			if (!found) {
				Q_EMIT selectionChangeFinished();
				return;
			}
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Event handling

void ot::GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) {
	QList<QGraphicsItem*> lst = items(_event->scenePos());
	for (auto itm : lst) {
		ot::GraphicsItem* actualItm = dynamic_cast<ot::GraphicsItem*>(itm);
		if (actualItm) {
			if (actualItm->getGraphicsItemFlags() & ot::GraphicsItemCfg::ItemIsConnectable) {
				this->startConnection(actualItm);
				QGraphicsScene::mouseDoubleClickEvent(_event);
				return;
			}
		}
	}

	this->stopConnection();

	QGraphicsScene::mouseDoubleClickEvent(_event);
}

void ot::GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_connectionPreview) {
		OTAssertNullptr(m_connectionOrigin);
		m_connectionPreview->setOriginPos(m_connectionOrigin->getQGraphicsItem()->scenePos() + m_connectionOrigin->getQGraphicsItem()->boundingRect().center());
		m_connectionPreview->setDestPos(_event->scenePos());
	}
	QGraphicsScene::mouseMoveEvent(_event);
}

void ot::GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	if (_event->button() == Qt::LeftButton) {
		m_lastSelection = this->selectedItems();
		m_mouseIsPressed = true;
	}
	QGraphicsScene::mousePressEvent(_event);
}

void ot::GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	QGraphicsScene::mouseReleaseEvent(_event);

	if (_event->button() == Qt::LeftButton) {
		m_mouseIsPressed = false;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Helper

void ot::GraphicsScene::drawBackground(QPainter* _painter, const QRectF& _rect)
{
	// Use QGraphicsScene method if no grid is set
	if (m_gridStepSize < 1) {
		return QGraphicsScene::drawBackground(_painter, _rect);
	}

	if (_rect != m_view->mapToScene(m_view->viewport()->rect()).boundingRect()) {
		m_view->update();
		return;
	}

	if (m_gridMode != NoGrid) { 
		this->drawGrid(_painter, _rect);
	}
}

void ot::GraphicsScene::drawGrid(QPainter* _painter, const QRectF& _rect) {
	// Setup pen
	qreal scaledLineWidth = this->calculateScaledLineWidth(_painter);

	QPen pen;
	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(OT_COLORSTYLE_VALUE_ControlsBorderColor).brush());
	pen.setWidthF(scaledLineWidth);

	QList<QLineF> normalLines;
	QList<QLineF> wideLines;
	QList<QLineF> centerLines;
	this->calculateGridLines(_rect, normalLines, wideLines, centerLines);

	_painter->setPen(pen);
	_painter->drawLines(normalLines);

	pen.setWidthF(scaledLineWidth * 3.);
	_painter->setPen(pen);
	_painter->drawLines(wideLines);

	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(OT_COLORSTYLE_VALUE_ControlsHoverBackground).brush());
	_painter->setPen(pen);
	_painter->drawLines(centerLines);

}

qreal ot::GraphicsScene::calculateScaledLineWidth(QPainter* _painter) const {
	// Get the current transformation matrix of the painter
	QTransform transform = _painter->transform();

	// Calculate the scaling factor from the transformation matrix
	qreal scaleX = qSqrt(transform.m11() * transform.m11() + transform.m21() * transform.m21());
	qreal scaleY = qSqrt(transform.m22() * transform.m22() + transform.m12() * transform.m12());

	qreal scale = qMax(scaleX, scaleY);

	return m_penWidth / scale;
}

void ot::GraphicsScene::calculateGridLines(const QRectF& _painterRect, QList<QLineF>& _normalLines, QList<QLineF>& _wideLines, QList<QLineF>& _centerLines) const {
	QLineF newLine;

	// Center line
	if (m_gridMode == BasicGridWithCenter || m_gridMode == AdvancedGrid) {
		if (0. >= _painterRect.left() && 0. <= _painterRect.right()) {
			_centerLines.push_back(QLineF(0., _painterRect.top(), 0., _painterRect.bottom()));
		}
		if (0. >= _painterRect.top() && 0. <= _painterRect.bottom()) {
			_centerLines.push_back(QLineF(_painterRect.left(), 0., _painterRect.right(), 0.));
		}
	}

	// X: Center to Left
	int startPos = std::min(0, (int)_painterRect.right() - ((_painterRect.right() < 0. ? ((int)_painterRect.right()) * (-1) : (int)_painterRect.right()) % m_gridStepSize));
	int lineCounter = startPos / m_gridStepSize;
	if (lineCounter < 0) lineCounter *= (-1);
	if (m_gridWideEvery > 1) lineCounter = (lineCounter % m_gridWideEvery);
	lineCounter--;
	for (int x = startPos; x > _painterRect.left(); x -= m_gridStepSize) 
	{
		if (m_gridMode == BasicGrid) {
			// Small line only
			_normalLines.push_back(QLineF((qreal)x, _painterRect.top(), (qreal)x, _painterRect.bottom()));
		}
		else if (m_gridMode == AdvancedGrid || m_gridMode == WideLinesOnly || m_gridMode == AdvancedGridNomalCenter) {
			// Wide line
			if (m_gridWideEvery > 1 && ++lineCounter >= m_gridWideEvery) {
				_wideLines.push_back(QLineF((qreal)x, _painterRect.top(), (qreal)x, _painterRect.bottom()));
				lineCounter = 0;
			}
			// Small line
			else if (m_gridMode != WideLinesOnly) {
				_normalLines.push_back(QLineF((qreal)x, _painterRect.top(), (qreal)x, _painterRect.bottom()));
			}
		}
	}

	// X: Center to Right
	startPos = std::max(0, (int)_painterRect.left() + ((_painterRect.left() < 0. ? ((int)_painterRect.left()) * (-1) : (int)_painterRect.left()) % m_gridStepSize));
	lineCounter = startPos / m_gridStepSize;
	if (lineCounter < 0) lineCounter *= (-1);
	if (m_gridWideEvery > 1) lineCounter = (lineCounter % m_gridWideEvery);
	lineCounter--;
	for (int x = startPos; x < _painterRect.right(); x += m_gridStepSize)
	{
		if (m_gridMode == BasicGrid) {
			// Small line only
			_normalLines.push_back(QLineF((qreal)x, _painterRect.top(), (qreal)x, _painterRect.bottom()));
		}
		else if (m_gridMode == AdvancedGrid || m_gridMode == WideLinesOnly || m_gridMode == AdvancedGridNomalCenter) {
			// Wide line
			if (m_gridWideEvery > 1 && ++lineCounter >= m_gridWideEvery) {
				_wideLines.push_back(QLineF((qreal)x, _painterRect.top(), (qreal)x, _painterRect.bottom()));
				lineCounter = 0;
			}
			// Small line
			else if (m_gridMode != WideLinesOnly) {
				_normalLines.push_back(QLineF((qreal)x, _painterRect.top(), (qreal)x, _painterRect.bottom()));
			}
		}
	}

	// Y: Center to Top
	startPos = std::min(0, (int)_painterRect.bottom() - ((_painterRect.bottom() < 0. ? ((int)_painterRect.bottom()) * (-1) : (int)_painterRect.bottom()) % m_gridStepSize));
	lineCounter = startPos / m_gridStepSize;
	if (lineCounter < 0) lineCounter *= (-1);
	if (m_gridWideEvery > 1) lineCounter = (lineCounter % m_gridWideEvery);
	lineCounter--;
	for (int y = startPos; y > _painterRect.top(); y -= m_gridStepSize)
	{
		if (m_gridMode == BasicGrid) {
			// Small line only
			_normalLines.push_back(QLineF(_painterRect.left(), (qreal)y, _painterRect.right(), (qreal)y));
		}
		else if (m_gridMode == AdvancedGrid || m_gridMode == WideLinesOnly || m_gridMode == AdvancedGridNomalCenter) {
			// Wide line
			if (m_gridWideEvery > 1 && ++lineCounter >= m_gridWideEvery) {
				_wideLines.push_back(QLineF(_painterRect.left(), (qreal)y, _painterRect.right(), (qreal)y));
				lineCounter = 0;
			}
			// Small line
			else if (m_gridMode != WideLinesOnly) {
				_normalLines.push_back(QLineF(_painterRect.left(), (qreal)y, _painterRect.right(), (qreal)y));
			}
		}
	}

	// Y: Center to Bottom
	startPos = std::max(0, (int)_painterRect.top() + ((_painterRect.top() < 0. ? ((int)_painterRect.top()) * (-1) : (int)_painterRect.top()) % m_gridStepSize));
	lineCounter = startPos / m_gridStepSize;
	if (lineCounter < 0) lineCounter *= (-1);
	if (m_gridWideEvery > 1) lineCounter = (lineCounter % m_gridWideEvery);
	lineCounter--;
	for (int y = startPos; y < _painterRect.bottom(); y += m_gridStepSize)
	{
		if (m_gridMode == BasicGrid) {
			// Small line only
			_normalLines.push_back(QLineF(_painterRect.left(), (qreal)y, _painterRect.right(), (qreal)y));
		}
		else if (m_gridMode == AdvancedGrid || m_gridMode == WideLinesOnly || m_gridMode == AdvancedGridNomalCenter) {
			// Wide line
			if (m_gridWideEvery > 1 && ++lineCounter >= m_gridWideEvery) {
				_wideLines.push_back(QLineF(_painterRect.left(), (qreal)y, _painterRect.right(), (qreal)y));
				lineCounter = 0;
			}
			// Small line
			else if (m_gridMode != WideLinesOnly) {
				_normalLines.push_back(QLineF(_painterRect.left(), (qreal)y, _painterRect.right(), (qreal)y));
			}
		}
	}
}