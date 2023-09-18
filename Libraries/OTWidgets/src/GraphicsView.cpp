//! @file GraphicsView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsFactory.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtCore/qmimedata.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qgraphicsproxywidget.h>
//#include <QtWidgets/qgraphicsscene.h>

ot::GraphicsView::GraphicsView() : m_isPressed(false), m_wheelEnabled(true), m_dropEnabled(false) {
	m_scene = new GraphicsScene(this);
	this->setScene(m_scene);
	this->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
	this->setAlignment(Qt::AlignAbsolute);

	// ToDo: This is required while working with proxy widgets, but we should not use em
	this->setMouseTracking(true);
}

ot::GraphicsView::~GraphicsView() {

}

// ########################################################################################################

void ot::GraphicsView::resetView(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = s->itemsBoundingRect();
	setSceneRect(QRectF());
	int w = boundingRect.width();
	int h = boundingRect.height();
	QRectF viewRect = boundingRect.marginsAdded(QMarginsF(w, h, w, h));
	fitInView(viewRect, Qt::AspectRatioMode::KeepAspectRatio);
	centerOn(viewRect.center());
}

void ot::GraphicsView::viewAll(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = mapFromScene(s->itemsBoundingRect()).boundingRect();
	int w = boundingRect.width();
	int h = boundingRect.height();
	QRect viewPortRect = viewport()->rect().marginsRemoved(QMargins(w, h, w, h));

	if (viewPortRect.width() > boundingRect.width() && viewPortRect.height() > boundingRect.height())
	{
		resetView();
	}
}

ot::GraphicsItem* ot::GraphicsView::getItem(const std::string&  _itemUid) {
	auto it = m_items.find(_itemUid);
	if (it == m_items.end()) {
		OT_LOG_WAS("Item with the UID \"" + _itemUid + "\" does not exist");
		return nullptr;
	}
	else {
		return it->second;
	}
}

ot::GraphicsConnectionItem* ot::GraphicsView::getConnection(const std::string& _connectionUid) {
	auto it = m_connections.find(_connectionUid);
	if (it == m_connections.end()) {
		OT_LOG_WAS("Connection with the UID \"" + _connectionUid + "\" does not exist");
		return nullptr;
	}
	else {
		return it->second;
	}
}

void ot::GraphicsView::addConnection(GraphicsItem* _origin, GraphicsItem* _dest) {
	ot::GraphicsConnectionItem* newConnection = new ot::GraphicsConnectionItem;
	QPen p;
	p.setColor(QColor(255, 0, 0));
	p.setWidth(1);
	
	m_scene->addItem(newConnection);

	m_connections.insert_or_assign(newConnection->graphicsItemUid(), newConnection);

	newConnection->connectItems(_origin, _dest);

	emit connectionCreated(newConnection);
}

void ot::GraphicsView::addItem(ot::GraphicsItem* _item) {
	auto it = m_items.find(_item->graphicsItemUid());
	if (it != m_items.end()) {
		OT_LOG_EAS("An item with the ID \"" + _item->graphicsItemUid() + "\" already exists in this view");
		delete _item;
		return;
	}

	m_items.insert_or_assign(_item->graphicsItemUid(), _item);
	m_scene->addItem(_item->getRootItem()->getQGraphicsItem());
}

// ########################################################################################################

// Protected: Slots

void ot::GraphicsView::wheelEvent(QWheelEvent* _event)
{
	if (!m_wheelEnabled) return;
	const ViewportAnchor anchor = transformationAnchor();
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	int angle = _event->angleDelta().y();
	qreal factor;
	if (angle > 0) {
		factor = 1.1;
	}
	else {
		factor = 0.9;
	}
	scale(factor, factor);
	update();

	setTransformationAnchor(anchor);

	viewAll();
}

void ot::GraphicsView::enterEvent(QEvent* _event)
{
	QGraphicsView::enterEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);
}

void ot::GraphicsView::mousePressEvent(QMouseEvent* _event)
{
	QGraphicsView::mousePressEvent(_event);
	if (_event->button() == Qt::MiddleButton) {
		viewport()->setCursor(Qt::ClosedHandCursor);
		m_lastPanPos = _event->pos();
		m_isPressed = true;
	}
	else if (_event->button() == Qt::LeftButton) {
		
		QGraphicsItem* itm = m_scene->itemAt(mapToScene(_event->pos()), QTransform());
		if (itm)
		{
			QGraphicsProxyWidget* proxy = dynamic_cast<QGraphicsProxyWidget*>(itm);
			if (proxy) {
				//proxy->setFlag(QGraphicsItem::ItemIsMovable);
				//proxy->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
			}
		}
	}
}

void ot::GraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
	QGraphicsView::mouseReleaseEvent(_event);

	if (_event->button() == Qt::MiddleButton) {
		m_isPressed = false;
		viewport()->setCursor(Qt::CrossCursor);
	}
	else if (_event->button() == Qt::LeftButton) {
		QGraphicsItem* itm = m_scene->itemAt(mapToScene(_event->pos()), QTransform());
		if (itm)
		{
			QGraphicsProxyWidget* proxy = dynamic_cast<QGraphicsProxyWidget*>(itm);
			if (proxy) {
				//proxy->setFlag(QGraphicsItem::ItemIsMovable, false);
				//proxy->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
			}
		}
	}
}

void ot::GraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
	if (m_isPressed) {
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (_event->x() - m_lastPanPos.x()));
		verticalScrollBar()->setValue(verticalScrollBar()->value() - (_event->y() - m_lastPanPos.y()));
		m_lastPanPos = _event->pos();
		_event->accept();
	}
	else {
		QGraphicsView::mouseMoveEvent(_event);
	}
}

void ot::GraphicsView::keyPressEvent(QKeyEvent* _event)
{
	if (_event->key() == Qt::Key_Space)
	{
		// Reset the view
		resetView();
	}
}

void ot::GraphicsView::keyReleaseEvent(QKeyEvent* _event) {}

void ot::GraphicsView::resizeEvent(QResizeEvent* _event)
{
	QGraphicsView::resizeEvent(_event);

	viewAll();
}

void ot::GraphicsView::dragEnterEvent(QDragEnterEvent* _event) {
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_Configuration).isEmpty() && m_dropEnabled) {
		_event->acceptProposedAction();
	}
	else {
		QGraphicsView::dragEnterEvent(_event);
	}
}

void ot::GraphicsView::dropEvent(QDropEvent* _event) {
	if (!m_dropEnabled) {
		_event->acceptProposedAction();
		return;
	}
	QByteArray cfgRaw = _event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_Configuration);
	if (cfgRaw.isEmpty()) {
		OT_LOG_W("Drop event reqected: MimeData not matching");
		return;
	}

	// Generate configuration from raw data
	ot::GraphicsItemCfg* cfg = nullptr;
	try {
		OT_rJSON_parseDOC(cfgDoc, cfgRaw.toStdString().c_str());
		OT_rJSON_val cfgObj = cfgDoc.GetObject();
		cfg = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(cfgObj);
		cfg->setFromJsonObject(cfgObj);
	}
	catch (const std::exception& e) {
		OT_LOG_EAS(e.what());
		if (cfg) delete cfg;
		cfg = nullptr;
		return;
	}
	catch (...) {
		OT_LOG_EA("Unknown error");
		if (cfg) delete cfg;
		cfg = nullptr;
		return;
	}

	if (cfg == nullptr) {
		OT_LOG_WA("No config created");
		return;
	}

	// Store current event position to position the new item at this pos
	QPointF position = this->mapToScene(_event->pos());

	// Create graphics item from configuration
	ot::GraphicsItem* newItem = nullptr;
	try {
		newItem = ot::GraphicsFactory::itemFromConfig(cfg);
		newItem->setGraphicsItemFlags(newItem->graphicsItemFlags() | ot::GraphicsItem::ItemNetworkContext | ot::GraphicsItem::ItemIsMoveable);
		
		// Move to new location
		newItem->getRootItem()->getQGraphicsItem()->setPos(position);

		emit itemCreated(newItem);
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		if (newItem) delete newItem;
		delete cfg;
		return;
	}
	catch (...) {
		OT_LOG_EA("Unknown error occured");
		if (newItem) delete newItem;
		delete cfg;
		return;
	}

	//newBlock->moveBy(position.x(), position.y());

	delete cfg;

	_event->acceptProposedAction();
}

void ot::GraphicsView::dragMoveEvent(QDragMoveEvent* _event) {
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_Configuration).isEmpty() && m_dropEnabled) {
		_event->acceptProposedAction();
	}
	else {
		QGraphicsView::dragMoveEvent(_event);
	}
}
