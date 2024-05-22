//! @file GraphicsView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsItemPreviewDrag.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtCore/qmimedata.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qgraphicsproxywidget.h>

ot::GraphicsView::GraphicsView(GraphicsScene* _scene) 
	: m_scene(_scene), m_isPressed(false), m_wheelEnabled(true), m_dropEnabled(false), m_stateChangeInProgress(false) 
{
	if (!m_scene) m_scene = new GraphicsScene(this);

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

void ot::GraphicsView::setGraphicsScene(GraphicsScene* _scene) {
	if (_scene == m_scene) return;
	if (!_scene) return;
	if (m_scene) delete m_scene;
	m_scene = _scene;
	this->setScene(_scene);
}

ot::GraphicsItem* ot::GraphicsView::getItem(const ot::UID&  _itemUid) {
	auto it = m_items.find(_itemUid);
	if (it == m_items.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

ot::GraphicsConnectionItem* ot::GraphicsView::getConnection(const ot::UID& _connectionUid) {
	auto it = m_connections.find(_connectionUid);
	if (it == m_connections.end()) {
		OT_LOG_WAS("Connection with the UID \"" + std::to_string(_connectionUid)+ "\" does not exist");
		return nullptr;
	}
	else {
		return it->second;
	}
}

bool ot::GraphicsView::connectionAlreadyExists(const ot::GraphicsConnectionCfg& _connection)
{
	bool alreadyExisting = false;
	for (auto& connection : m_connections)
	{
		if (connection.second->getConnectionKey() == _connection.createConnectionKey() || connection.second->getConnectionKey() == _connection.createConnectionKeyReverse())
		{
			alreadyExisting = true;
			break;
		}
	}
	return (alreadyExisting);
}

void ot::GraphicsView::addItem(ot::GraphicsItem* _item) {
	auto it = m_items.find(_item->graphicsItemUid());
	bool removeConnectionBufferApplied = false;
	if (it != m_items.end()) {
		OT_LOG_D("Overwriting item with the ID \"" + std::to_string(_item->graphicsItemUid()));
		removeConnectionBufferApplied = true;
		this->removeItem(_item->graphicsItemUid(),removeConnectionBufferApplied);
	}

	m_items.insert_or_assign(_item->graphicsItemUid(), _item);
	m_scene->addItem(_item->getRootItem()->getQGraphicsItem());
	_item->getRootItem()->getQGraphicsItem()->setZValue(1);
	_item->setGraphicsScene(m_scene);

	if (removeConnectionBufferApplied)
	{
		for (const GraphicsConnectionCfg& bufferedConnection : m_itemRemovalConnectionBuffer) {
			addConnection(bufferedConnection);
		}
		m_itemRemovalConnectionBuffer.clear();
	}

	auto currentConnection = m_connectionCreationBuffer.begin();
	while (currentConnection !=  m_connectionCreationBuffer.end())
	{
		const bool connectionCreated = addConnectionIfConnectedItemsExist(*currentConnection);
		if (connectionCreated)
		{
			currentConnection =	m_connectionCreationBuffer.erase(currentConnection);
		}
		else
		{
			currentConnection++;
		}
	}
}

void ot::GraphicsView::removeItem(const ot::UID& _itemUid, bool bufferConnections) {
	auto graphicsItemByUID = m_items.find(_itemUid);
	if (graphicsItemByUID == m_items.end()) {
		//OT_LOG_EAS("Item with the ID \"" + _itemUid + "\" could not be found");
		return;
	}
	m_stateChangeInProgress = true;
	ot::GraphicsItem* graphicsItem =  graphicsItemByUID->second;
	if (bufferConnections)
	{
		m_itemRemovalConnectionBuffer = graphicsItem->getConnectionCfgs();
		m_itemRemovalConnectionBuffer.unique();
	}
	graphicsItem->removeAllConnections();
	m_scene->removeItem(graphicsItem->getQGraphicsItem());
	delete graphicsItem;
	m_items.erase(_itemUid);
	m_stateChangeInProgress = false;
}

std::list<ot::UID> ot::GraphicsView::selectedItems(void) const {
	std::list<ot::UID> sel; // Selected items
	for (auto s : m_scene->selectedItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(s);
		ot::GraphicsConnectionItem* citm = dynamic_cast<ot::GraphicsConnectionItem*>(s);

		if (itm) {
			// Item selected
			sel.push_back(itm->graphicsItemUid());
		}
	}
	return sel;
}

bool ot::GraphicsView::addConnectionIfConnectedItemsExist(const GraphicsConnectionCfg& _config)
{
	if (connectedGraphicItemsExist(_config))
	{
		addConnection(_config);
		return true;
	}
	else
	{
		if (std::find(m_connectionCreationBuffer.begin(), m_connectionCreationBuffer.end(), _config) == m_connectionCreationBuffer.end())
		{
			m_connectionCreationBuffer.push_back(_config);
		}
		return false;
	}
}

void ot::GraphicsView::addConnection(const GraphicsConnectionCfg& _config) {
	this->removeConnection(_config.getUid());
	ot::GraphicsItem* src = this->getItem(_config.getOriginUid());
	ot::GraphicsItem* dest = this->getItem(_config.getDestinationUid());
	ot::GraphicsItem* srcConn = src->findItem(_config.originConnectable());
	ot::GraphicsItem* destConn = dest->findItem(_config.destConnectable());

	if (!srcConn || !destConn) {
		OT_LOG_EA("Invalid connectable name");
		return;
	}

	// Create and add new connection
	ot::GraphicsConnectionItem* newConnection = new ot::GraphicsConnectionItem;
	newConnection->setupFromConfig(_config);
	
	m_scene->addItem(newConnection);
	//newConnection->setGraphicsScene(m_scene);
	newConnection->connectItems(srcConn, destConn);
	newConnection->setZValue(0);

	m_connections.insert_or_assign(_config.getUid(), newConnection);
}

bool ot::GraphicsView::connectedGraphicItemsExist(const GraphicsConnectionCfg& _config)
{
	ot::GraphicsItem* src = this->getItem(_config.getOriginUid());
	ot::GraphicsItem* dest = this->getItem(_config.getDestinationUid());

	return src != nullptr && dest != nullptr;
}

void ot::GraphicsView::removeConnection(const GraphicsConnectionCfg& _connectionInformation) {
	this->removeConnection(_connectionInformation.getUid());
}

void ot::GraphicsView::removeConnection(const ot::UID& _connectionUID)
{
	// Ensure connection exists
	auto connectionByUID = m_connections.find(_connectionUID);
	if (connectionByUID == m_connections.end()) {
		return;
	}
	m_stateChangeInProgress = true;

	// Remove connection from items
	ot::GraphicsConnectionItem* connection = connectionByUID->second;
	connection->disconnectItems();

	// Destroy connection
	delete connection;
	connection = nullptr;

	// Erase connection from map
	m_connections.erase(_connectionUID);
	m_stateChangeInProgress = false;
}

ot::UIDList ot::GraphicsView::selectedConnections(void) const {
	ot::UIDList sel; // Selected items
	for (auto s : m_scene->selectedItems()) {
		ot::GraphicsConnectionItem* citm = dynamic_cast<ot::GraphicsConnectionItem*>(s);

		if (citm) {
			// Connection selected
			sel.push_back(citm->getConnectionInformation().getUid());
		}
	}

	return sel;
}

void ot::GraphicsView::requestConnection(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector) {
	if (this->connectionAlreadyExists(ot::GraphicsConnectionCfg(_fromUid, _fromConnector, _toUid, _toConnector))) {
		OT_LOG_W("Connection already exists { \"Origin.UID\": \"" + std::to_string(_fromUid) + "\", \"Origin.Conn\"" + _fromConnector + "\", \"Dest.UID\": \"" + std::to_string(_toUid) + "\", \"Dest.Conn\": \"" + _toConnector + "\" }");
		return;
	}
	Q_EMIT connectionRequested(_fromUid, _fromConnector, _toUid, _toConnector);
}

void ot::GraphicsView::notifyItemMoved(ot::GraphicsItem* _item) {
	Q_EMIT itemMoved(_item->graphicsItemUid(), _item->getQGraphicsItem()->pos());
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
	else if (_event->key() == Qt::Key_Delete) {
		ot::UIDList itm = this->selectedItems();
		ot::UIDList con = this->selectedConnections();
		Q_EMIT removeItemsRequested(itm, con);
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
	if (!_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName).isEmpty() && m_dropEnabled) {
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
	QString itemName = _event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName);
	if (itemName.isEmpty()) {
		OT_LOG_W("Drop event reqected: MimeData not matching");
		return;
	}

	Q_EMIT itemRequested(itemName, this->mapToScene(_event->pos()));
	_event->acceptProposedAction();
}

void ot::GraphicsView::dragMoveEvent(QDragMoveEvent* _event) {
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName).isEmpty() && m_dropEnabled) {
		_event->acceptProposedAction();
	}
	else {
		QGraphicsView::dragMoveEvent(_event);
	}
}
