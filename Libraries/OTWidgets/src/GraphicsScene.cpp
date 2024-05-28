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
	m_gridFlags(NoGridFlags), m_gridSnapEnabled(true)
{
	this->connect(this, &GraphicsScene::selectionChanged, this, &GraphicsScene::slotSelectionChanged);
}

ot::GraphicsScene::GraphicsScene(const QRectF& _sceneRect, GraphicsView* _view)
	: QGraphicsScene(_sceneRect), m_gridStepSize(10), m_gridWideEvery(10), m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr),
	m_connectionPreviewStyle(ot::GraphicsConnectionCfg::DirectLine), m_ignoreEvents(false), m_mouseIsPressed(false),
	m_gridFlags(NoGridFlags), m_gridSnapEnabled(true)
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

// Setter / Getter

QPointF ot::GraphicsScene::snapToGrid(const QPointF& _pos) const {
	return this->snapToGrid(_pos, m_gridStepSize);
}

QPointF ot::GraphicsScene::snapToGrid(const QPointF& _pos, int _gridStepSize) const {
	if (m_gridSnapEnabled && (_gridStepSize > 0)) {
		QPointF pt = _pos;
		pt.setX(round(pt.x()));
		pt.setY(round(pt.y()));
		if (pt.x() < 0.) {
			pt.setX(pt.x() + (qreal)(((int)pt.x() * (-1)) % _gridStepSize));
		}
		else {
			pt.setX(pt.x() - (qreal)(((int)pt.x()) % _gridStepSize));
		}
		if (pt.y() < 0.) {
			pt.setY(pt.y() + (qreal)(((int)pt.y() * (-1)) % _gridStepSize));
		}
		else {
			pt.setY(pt.y() - (qreal)(((int)pt.y()) % _gridStepSize));
		}
		return pt;
	}
	else {
		return _pos;
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

	if ((m_gridFlags | NoGridLineMask) != NoGridLineMask) {
		this->drawGrid(_painter, _rect);
	}
}

void ot::GraphicsScene::drawGrid(QPainter* _painter, const QRectF& _rect) {
	QList<QLineF> normalLines;
	QList<QLineF> wideLines;
	QList<QLineF> centerLines;
	this->calculateGridLines(_rect, normalLines, wideLines, centerLines);

	// Setup pen
	QPen pen = m_defaultGridPen;
	qreal scaledLineWidth = this->calculateScaledGridLineWidth(_painter);

	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(OT_COLORSTYLE_VALUE_ControlsBorderColor).brush());
	pen.setWidthF(scaledLineWidth);
	
	if (!normalLines.isEmpty()) {
		_painter->setPen(pen);
		_painter->drawLines(normalLines);
	}

	if (!wideLines.isEmpty()) {
		pen.setWidthF(scaledLineWidth * 3.);
		_painter->setPen(pen);
		_painter->drawLines(wideLines);
	}
	
	if (!centerLines.isEmpty()) {
		pen.setWidthF(scaledLineWidth * 3.);
		pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(OT_COLORSTYLE_VALUE_ControlsHoverBackground).brush());
		_painter->setPen(pen);
		_painter->drawLines(centerLines);
	}
}

qreal ot::GraphicsScene::calculateScaledGridLineWidth(QPainter* _painter) const {
	// Get the current transformation matrix of the painter
	QTransform transform = _painter->transform();

	// Calculate the scaling factor from the transformation matrix
	qreal scaleX = qSqrt(transform.m11() * transform.m11() + transform.m21() * transform.m21());
	qreal scaleY = qSqrt(transform.m22() * transform.m22() + transform.m12() * transform.m12());

	qreal scale = qMax(scaleX, scaleY);

	return m_defaultGridPen.widthF() / scale;
}

void ot::GraphicsScene::calculateGridLines(const QRectF& _painterRect, QList<QLineF>& _normalLines, QList<QLineF>& _wideLines, QList<QLineF>& _centerLines) const {
	if ((m_gridFlags | NoGridLineMask) == NoGridLineMask) return;
	
	// Center line
	if (m_gridFlags & GraphicsScene::ShowCenterCross) {
		if (0. >= _painterRect.left() && 0. <= _painterRect.right()) {
			_centerLines.push_back(QLineF(0., _painterRect.top(), 0., _painterRect.bottom()));
		}
		if (0. >= _painterRect.top() && 0. <= _painterRect.bottom()) {
			_centerLines.push_back(QLineF(_painterRect.left(), 0., _painterRect.right(), 0.));
		}
	}

	// Scale the grid step size if AutoScaleGrid is set
	

	// Get starting point
	qreal scaledStepSize = this->calculateScaledGridStepSize(_painterRect);
	QPointF startPos = this->snapToGrid(_painterRect.topLeft(), scaledStepSize);

	int lineCounterX = 0;
	int lineCounterY = 0;


	// If wide lines will be displayed the line counter need to be calculated
	if ((m_gridFlags & GraphicsScene::ShowWideLines) && m_gridWideEvery > 1) {

		// Calculate initial line counter for X axis
		lineCounterX = (int)(startPos.x() / scaledStepSize);
		if (lineCounterX < 0) {
			lineCounterX *= (-1);
		}
		if (startPos.x() <= _painterRect.left()) {
			if (startPos.x() < 0.) {
				lineCounterX--;
			}
			else {
				lineCounterX++;
			}
			startPos.setX(startPos.x() + scaledStepSize);
		}

		// Calculate initial line counter for Y axis
		lineCounterY = (int)(startPos.y() / scaledStepSize);
		if (lineCounterY < 0) {
			lineCounterY *= (-1);
		}
		if (startPos.y() <= _painterRect.top()) {
			if (startPos.y() < 0.) {
				lineCounterY--;
			}
			else {
				lineCounterY++;
			}
			startPos.setY(startPos.y() + scaledStepSize);
		}

		// And now use modulo to get the current counter value in the counter range
		lineCounterX = (lineCounterX % m_gridWideEvery);
		lineCounterY = (lineCounterY % m_gridWideEvery);
	}

	// Calculate X axis lines
	for (qreal x = startPos.x(); x < _painterRect.right(); x += scaledStepSize)
	{
		if (m_gridFlags & GraphicsScene::ShowNormalLines) {
			_normalLines.push_back(QLineF(x, _painterRect.top(), x, _painterRect.bottom()));
		}
		if ((m_gridFlags & GraphicsScene::ShowWideLines) && (m_gridWideEvery > 1)) {
			// Wide line
			if (lineCounterX >= m_gridWideEvery) {
				_wideLines.push_back(QLineF(x, _painterRect.top(), x, _painterRect.bottom()));
				lineCounterX = 0;
			}
			lineCounterX++;
		}
	}

	// Calculate Y axis lines	
	for (qreal y = startPos.y(); y < _painterRect.bottom(); y += scaledStepSize)
	{
		if (m_gridFlags & GraphicsScene::ShowNormalLines) {
			// Small line only
			_normalLines.push_back(QLineF(_painterRect.left(), y, _painterRect.right(), y));
		}
		if ((m_gridFlags & GraphicsScene::ShowWideLines) && (m_gridWideEvery > 1)) {
			// Wide line
			if (lineCounterY >= m_gridWideEvery) {
				_wideLines.push_back(QLineF(_painterRect.left(), y, _painterRect.right(), y));
				lineCounterY = 0;
			}
			lineCounterY++;
		}
	}
}

qreal ot::GraphicsScene::calculateScaledGridStepSize(const QRectF& _rect) const {
	qreal scaledStepSize = m_gridStepSize;
	if (m_gridFlags & AutoScaleGrid) {
		while (((_rect.width() / scaledStepSize) > 200) || ((_rect.height() / scaledStepSize) > 200)) {
			scaledStepSize *= 10.;
		}
	}
	return scaledStepSize;
}