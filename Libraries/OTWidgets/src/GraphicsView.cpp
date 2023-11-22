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
#include "OTWidgets/GraphicsConnectionItem.h"

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

	this->setUpdatesEnabled(true);
}

ot::GraphicsView::~GraphicsView() {

}

// ########################################################################################################

void ot::GraphicsView::resetView(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = s->itemsBoundingRect();
	this->setSceneRect(QRectF());
	int w = boundingRect.width();
	int h = boundingRect.height();
	QRectF viewRect = boundingRect.marginsAdded(QMarginsF(w, h, w, h));
	this->fitInView(viewRect, Qt::AspectRatioMode::KeepAspectRatio);
	this->centerOn(viewRect.center());
}

void ot::GraphicsView::fitInCurrentView(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = s->itemsBoundingRect();
	this->setSceneRect(boundingRect);
	this->fitInView(boundingRect, Qt::AspectRatioMode::KeepAspectRatio);
	this->centerOn(boundingRect.center());
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
		OT_LOG_D("Item with the UID \"" + _itemUid + "\" does not exist");
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

bool ot::GraphicsView::connectionAlreadyExists(const ot::GraphicsConnectionCfg& _connection)
{
	return (m_connections.find(_connection.buildKey()) != m_connections.end() || m_connections.find(_connection.buildReversedKey()) != m_connections.end()) ;
}

void ot::GraphicsView::addItem(ot::GraphicsItem* _item) {
	auto it = m_items.find(_item->graphicsItemUid());
	if (it != m_items.end()) {
		OT_LOG_D("Overwriting item with the ID \"" + _item->graphicsItemUid());
		this->removeItem(_item->graphicsItemUid());
	}

	m_items.insert_or_assign(_item->graphicsItemUid(), _item);
	m_scene->addItem(_item->getRootItem()->getQGraphicsItem());
	_item->setGraphicsScene(m_scene);
}

void ot::GraphicsView::removeItem(const std::string& _itemUid) {
	auto it = m_items.find(_itemUid);
	if (it == m_items.end()) {
		//OT_LOG_EAS("Item with the ID \"" + _itemUid + "\" could not be found");
		return;
	}

	it->second->removeAllConnections();
	m_scene->removeItem(it->second->getQGraphicsItem());
	delete it->second;
	m_items.erase(_itemUid);
}

void ot::GraphicsView::addConnection(GraphicsItem* _origin, GraphicsItem* _dest, const GraphicsConnectionCfg& _config) {
	ot::GraphicsConnectionItem* newConnection = new ot::GraphicsConnectionItem;
	newConnection->setupFromConfig(_config);
	
	m_scene->addItem(newConnection);
	//newConnection->setGraphicsScene(m_scene);
	newConnection->connectItems(_origin, _dest);

	std::string itmKey = ot::GraphicsConnectionCfg::buildKey(_origin->getRootItem()->graphicsItemUid(), _origin->graphicsItemName(), _dest->getRootItem()->graphicsItemUid(), _dest->graphicsItemName());
	m_connections.insert_or_assign(itmKey, newConnection);
}

void ot::GraphicsView::removeConnection(const GraphicsConnectionCfg& _connectionInformation) {
	auto it = m_connections.find(_connectionInformation.buildKey());
	if (it == m_connections.end()) {
		it = m_connections.find(_connectionInformation.buildReversedKey());
		if (it == m_connections.end()) {
			OT_LOG_W("Connection not found { \"OriginUID\": \"" + _connectionInformation.originUid() + 
				"\", \"OriginConnectable\": \"" + _connectionInformation.originConnectable() + 
				"\", \"DestUID\": \"" + _connectionInformation.destUid() + 
				"\", \"DestConnectable\": \"" + _connectionInformation.destConnectable() + "\" }");
			return;
		}
	}

	// Remove connection from items
	it->second->disconnectItems();

	// Remove connection from view
	m_scene->removeItem(it->second);

	// Destroy connection
	delete it->second;

	// Erase connection from map
	m_connections.erase(_connectionInformation.buildKey());
	m_connections.erase(_connectionInformation.buildReversedKey());
}

void ot::GraphicsView::removeConnection(const std::string& _fromUid, const std::string& _fromConnector, const std::string& _toUid, const std::string& _toConnector) {
	this->removeConnection(GraphicsConnectionCfg(_fromUid, _fromConnector, _toUid, _toConnector));
}

void ot::GraphicsView::requestConnection(const std::string& _fromUid, const std::string& _fromConnector, const std::string& _toUid, const std::string& _toConnector) {
	if (this->connectionAlreadyExists(ot::GraphicsConnectionCfg(_fromUid, _fromConnector, _toUid, _toConnector))) {
		OT_LOG_W("Connection already exists { \"Origin.UID\": \"" + _fromUid + "\", \"Origin.Conn\"" + _fromConnector + "\", \"Dest.UID\": \"" + _toUid + "\", \"Dest.Conn\": \"" + _toConnector + "\" }");
		return;
	}
	emit connectionRequested(_fromUid, _fromConnector, _toUid, _toConnector);
}

void ot::GraphicsView::notifyItemMoved(ot::GraphicsItem* _item) {
	emit itemMoved(_item->graphicsItemUid(), _item->getQGraphicsItem()->pos());
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
	this->scale(factor, factor);
	this->update();

	this->setTransformationAnchor(anchor);

	this->viewAll();
}

void ot::GraphicsView::enterEvent(QEvent* _event)
{
	QGraphicsView::enterEvent(_event);
	this->viewport()->setCursor(Qt::CrossCursor);
}

void ot::GraphicsView::mousePressEvent(QMouseEvent* _event)
{
	QGraphicsView::mousePressEvent(_event);
	if (_event->button() == Qt::MiddleButton) {
		this->viewport()->setCursor(Qt::ClosedHandCursor);
		m_lastPanPos = _event->pos();
		m_isPressed = true;
	}
}

void ot::GraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
	QGraphicsView::mouseReleaseEvent(_event);

	if (_event->button() == Qt::MiddleButton) {
		m_isPressed = false;
		this->viewport()->setCursor(Qt::CrossCursor);
	}
}

void ot::GraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
	if (m_isPressed) {
		this->horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (_event->x() - m_lastPanPos.x()));
		this->verticalScrollBar()->setValue(verticalScrollBar()->value() - (_event->y() - m_lastPanPos.y()));
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
		this->resetView();
	}
}

void ot::GraphicsView::keyReleaseEvent(QKeyEvent* _event) {}

void ot::GraphicsView::resizeEvent(QResizeEvent* _event)
{
	QGraphicsView::resizeEvent(_event);

	this->viewAll();
}

void ot::GraphicsView::dragEnterEvent(QDragEnterEvent* _event) {
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_ItemName).isEmpty() && m_dropEnabled) {
		_event->acceptProposedAction();
	}
	else {
		QGraphicsView::dragEnterEvent(_event);
	}
}

void ot::GraphicsView::dropEvent(QDropEvent* _event) {
	if (!m_dropEnabled) {
		return;
	}
	QString itemName = _event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_ItemName);
	if (itemName.isEmpty()) {
		OT_LOG_W("Drop event reqected: MimeData not matching");
		return;
	}

	emit itemRequested(itemName, this->mapToScene(_event->pos()));
	_event->acceptProposedAction();
}

void ot::GraphicsView::dragMoveEvent(QDragMoveEvent* _event) {
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_ItemName).isEmpty() && m_dropEnabled) {
		_event->acceptProposedAction();
	}
	else {
		QGraphicsView::dragMoveEvent(_event);
	}
}
