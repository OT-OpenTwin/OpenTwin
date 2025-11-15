// @otlicense
// File: GraphicsView.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/EntityName.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsItemPreviewDrag.h"
#include "OTWidgets/GraphicsConnectionConnectorItem.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qclipboard.h>
#include <QtCore/qmimedata.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qgraphicsproxywidget.h>

ot::GraphicsView::GraphicsView(QWidget* _parent) : GraphicsView(nullptr, _parent) {}

ot::GraphicsView::GraphicsView(GraphicsScene* _scene, QWidget* _parent) :
	QGraphicsView(_parent), m_scene(_scene), m_wheelEnabled(true), m_dropEnabled(false),
	m_viewFlags(NoViewFlags), m_viewStateFlags(DefaultState), m_sceneMargins(5., 5., 5., 5.)
{
	if (!m_scene) {
		m_scene = new GraphicsScene(this);
	}

	this->setScene(m_scene);
	this->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
	this->setAlignment(Qt::AlignAbsolute);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	this->setUpdatesEnabled(true);

	this->connect(m_scene, &GraphicsScene::graphicsItemDoubleClicked, this, &GraphicsView::slotGraphicsItemDoubleClicked);

	QShortcut* copyShortcut = new QShortcut(this);
	copyShortcut->setKey(QKeySequence("Ctrl+C"));
	copyShortcut->setContext(Qt::WidgetWithChildrenShortcut);
	this->connect(copyShortcut, &QShortcut::activated, this, &GraphicsView::slotCopy);

	QShortcut* pasteShortcut = new QShortcut(this);
	pasteShortcut->setKey(QKeySequence("Ctrl+V"));
	pasteShortcut->setContext(Qt::WidgetWithChildrenShortcut);
	this->connect(pasteShortcut, &QShortcut::activated, this, &GraphicsView::slotPaste);
}

ot::GraphicsView::~GraphicsView() {
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsView::setGraphicsViewName(const std::string& _name) {
	m_viewName = _name;

	// Reparent items
	for (auto& it : m_items) {
		if (EntityName::isChildOf(it.second->getGraphicsItemName(), m_viewName)) {
			it.second->setGraphicsItemName(EntityName::changeParentPath(it.second->getGraphicsItemName(), m_viewName));
		}
	}
}

void ot::GraphicsView::setGraphicsScene(GraphicsScene* _scene) {
	OTAssertNullptr(_scene);

	if (_scene == m_scene) {
		return;
	}

	if (m_scene) {
		delete m_scene;
	}

	m_scene = _scene;

	this->setScene(_scene);
}

// ###########################################################################################################################################################################################################################################################################################################################

// View handling

void ot::GraphicsView::resetView() {
	OTAssertNullptr(m_scene);
	QRectF boundingRect = m_scene->itemsBoundingRect().marginsAdded(m_sceneMargins);
	if (m_viewFlags & ViewManagesSceneRect) {
		this->setSceneRect(boundingRect.marginsAdded(m_sceneMargins));
	}

	this->fitInView(boundingRect, Qt::AspectRatioMode::KeepAspectRatio);
	this->centerOn(boundingRect.center());
}

void ot::GraphicsView::ensureViewInBounds() {
	OTAssertNullptr(m_scene);
	QRect itemsRect = this->mapFromScene(m_scene->itemsBoundingRect().marginsAdded(m_sceneMargins)).boundingRect();
	QRect viewPortRect = this->viewport()->rect();

	if (itemsRect.width() < viewPortRect.width() && itemsRect.height() < viewPortRect.height()) {
		this->resetView();
	}
}

QRectF ot::GraphicsView::getVisibleSceneRect() const {
	return this->mapToScene(this->viewport()->rect()).boundingRect();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Item handling

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

void ot::GraphicsView::renameItem(const std::string& _oldEntityName, const std::string& _newEntityName) {
	for (auto& it : m_items) {
		if (it.second->getGraphicsItemName() == _oldEntityName) {
			it.second->setGraphicsItemName(_newEntityName);
		}
	}
}

bool ot::GraphicsView::connectionAlreadyExists(const ot::GraphicsConnectionCfg& _connection)
{
	bool alreadyExisting = false;
	for (auto& connection : m_connections)
	{
		if (connection.second->getConfiguration().createConnectionKey() == _connection.createConnectionKey() || connection.second->getConfiguration().createConnectionKey() == _connection.createConnectionKeyReverse())
		{
			alreadyExisting = true;
			break;
		}
	}
	return alreadyExisting;
}

void ot::GraphicsView::addItem(ot::GraphicsItem* _item) {
	auto it = m_items.find(_item->getGraphicsItemUid());
	if (it != m_items.end()) {
		OT_LOG_D("Overwriting item with the ID \"" + std::to_string(_item->getGraphicsItemUid()));
		this->removeItem(_item->getGraphicsItemUid());
	}

	_item->setBlockConfigurationNotifications(true);

	m_items.insert_or_assign(_item->getGraphicsItemUid(), _item);
	m_scene->addItem(_item->getRootItem()->getQGraphicsItem());
	_item->setGraphicsScene(m_scene);

	QPointF pt = m_scene->snapToGrid(_item);
	if (pt != _item->getQGraphicsItem()->pos()) {
		_item->setGraphicsItemPos(pt);
	}

	// Check for max trigger distance
	m_scene->checkMaxTriggerDistance(_item->getMaxAdditionalTriggerDistance());

	_item->setBlockConfigurationNotifications(false);

	OT_LOG_D("Item added { \"UID\": " + std::to_string(_item->getGraphicsItemUid()) + " }");

	// Check if any connection in the buffer can be created now
	for (auto connection : m_connections) {
		if (connection.second->getConfiguration().getOriginUid() == _item->getGraphicsItemUid()) {
			GraphicsItem* connector = _item->findItem(connection.second->getConfiguration().getOriginConnectable());
			if (connector) {
				connection.second->setOriginItem(connector);
			}
		}
		else if (connection.second->getConfiguration().getDestinationUid() == _item->getGraphicsItemUid()) {
			GraphicsItem* connector = _item->findItem(connection.second->getConfiguration().getDestConnectable());
			if (connector) {
				connection.second->setDestItem(connector);
			}
		}
	}
}

void ot::GraphicsView::removeItem(const ot::UID& _itemUid) {
	auto graphicsItemByUID = m_items.find(_itemUid);
	if (graphicsItemByUID == m_items.end()) {
		return;
	}
	
	QSignalBlocker viewBlocker(this);
	QSignalBlocker sceneBlocker(m_scene);

	ot::GraphicsItem* graphicsItem =  graphicsItemByUID->second;
	OTAssertNullptr(graphicsItem);

	for (GraphicsConnectionItem* connection : graphicsItem->getAllConnections()) {
		// Removing an item may occur during undo/redo operations, therefore we don't want to update the config
		connection->disconnectItem(graphicsItem, false);
	}

	m_scene->itemAboutToBeRemoved(graphicsItem);
	//graphicsItem->removeAllConnections();
	m_scene->removeItem(graphicsItem->getQGraphicsItem());

	graphicsItem->setGraphicsScene(nullptr);
	delete graphicsItem;
	graphicsItem = nullptr;
	m_items.erase(_itemUid);
	
	OT_LOG_D("Item removed { \"UID\": " + std::to_string(_itemUid) + " }");
}

void ot::GraphicsView::setSelectedElements(const ot::UIDList& _uids) {
	QSignalBlocker blocker(this);
	QSignalBlocker blocker2(m_scene);

	for (auto& item : m_items) {
		item.second->getQGraphicsItem()->setSelected(std::find(_uids.begin(), _uids.end(), item.second->getGraphicsItemUid()) != _uids.end());
	}

	for (auto& conn : m_connections) {
		conn.second->getQGraphicsItem()->setSelected(std::find(_uids.begin(), _uids.end(), conn.second->getConfiguration().getUid()) != _uids.end());
	}
}

std::list<ot::UID> ot::GraphicsView::getSelectedItemUIDs() const {
	std::list<ot::UID> sel; // Selected items
	for (auto s : m_scene->selectedItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(s);
		ot::GraphicsConnectionItem* citm = dynamic_cast<ot::GraphicsConnectionItem*>(s);

		if (itm) {
			// Item selected
			sel.push_back(itm->getGraphicsItemUid());
		}
	}
	return sel;
}

std::list<ot::GraphicsItem*> ot::GraphicsView::getSelectedGraphicsItems() const {
	std::list<GraphicsItem*> sel; // Selected items
	for (auto s : m_scene->selectedItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(s);

		if (itm) {
			// Item selected
			sel.push_back(itm);
		}
	}
	return sel;
}

void ot::GraphicsView::setGraphicsSceneRect(const QRectF& _rect) {
	this->setSceneRect(_rect);
	QRectF rec = _rect.marginsAdded(m_sceneMargins);
	this->fitInView(rec, Qt::KeepAspectRatio);
	this->centerOn(rec.center());
	this->horizontalScrollBar()->setRange(rec.left(), rec.right());
	this->verticalScrollBar()->setRange(rec.top(), rec.bottom());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Connection handling

void ot::GraphicsView::addConnection(const GraphicsConnectionCfg& _config) {
	this->removeConnection(_config.getUid());

	ot::GraphicsItem* src = this->getItem(_config.getOriginUid());
	ot::GraphicsItem* dest = this->getItem(_config.getDestinationUid());
	
	ot::GraphicsItem* srcConn = nullptr;
	ot::GraphicsItem* destConn = nullptr;

	// Create and add new connection
	ot::GraphicsConnectionItem* newConnection = new ot::GraphicsConnectionItem;
	m_scene->addItem(newConnection);
	newConnection->setGraphicsScene(m_scene);

	newConnection->setConfiguration(_config);
	
	if (src) {
		srcConn = src->findItem(_config.getOriginConnectable());
		if (!srcConn) {
			OT_LOG_E("Origin connector not found { \"ItemUID\": " + std::to_string(_config.getOriginUid()) + ", \"Connector\": \"" + _config.getOriginConnectable() + "\" }");
		}
	}
	if (dest) {
		destConn = dest->findItem(_config.getDestConnectable());
		if (!destConn) {
			OT_LOG_E("Destination connector not found { \"ItemUID\": " + std::to_string(_config.getDestinationUid()) + ", \"Connector\": \"" + _config.getDestConnectable() + "\" }");
		}
	}

	if (srcConn) {
		newConnection->setOriginItem(srcConn);
	}
	else {
		newConnection->setOriginPos(_config.getOriginPos());
	}
	if (destConn) {
		newConnection->setDestItem(destConn);
	}
	else {
		newConnection->setDestPos(_config.getDestPos());
	}

	m_connections.insert_or_assign(_config.getUid(), newConnection);

	OT_LOG_D("New connection added { \"UID\": " + std::to_string(_config.getUid()) + " }");
}

void ot::GraphicsView::removeConnection(const GraphicsConnectionCfg& _connectionInformation) {
	this->removeConnection(_connectionInformation.getUid());
}

void ot::GraphicsView::removeConnection(const ot::UID& _connectionUID) {
	// Ensure connection exists
	auto connectionByUID = m_connections.find(_connectionUID);
	if (connectionByUID == m_connections.end()) {
		return;
	}

	this->blockSignals(true);
	m_scene->blockSignals(true);

	// Remove connection from items
	ot::GraphicsConnectionItem* connection = connectionByUID->second;
	OTAssertNullptr(connection);

	m_scene->connectionAboutToBeRemoved(connection);
	connection->disconnectItems(false);

	// Destroy connection
	m_scene->removeItem(connection);
	delete connection;
	connection = nullptr;

	// Erase connection from map
	m_connections.erase(_connectionUID);
	
	OT_LOG_D("Connection removed { \"UID\": " + std::to_string(_connectionUID) + " }");

	m_scene->blockSignals(false);
	this->blockSignals(false);
}

ot::UIDList ot::GraphicsView::getSelectedConnectionUIDs() const {
	ot::UIDList sel; // Selected items
	for (auto s : m_scene->selectedItems()) {
		ot::GraphicsConnectionItem* citm = dynamic_cast<ot::GraphicsConnectionItem*>(s);

		if (citm) {
			// Connection selected
			sel.push_back(citm->getConfiguration().getUid());
		}
	}

	return sel;
}

std::list<ot::GraphicsConnectionItem*> ot::GraphicsView::getSelectedConnectionItems() const {
	std::list<GraphicsConnectionItem*> sel; // Selected items
	for (auto s : m_scene->selectedItems()) {
		ot::GraphicsConnectionItem* citm = dynamic_cast<ot::GraphicsConnectionItem*>(s);

		if (citm) {
			// Connection selected
			sel.push_back(citm);
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

void ot::GraphicsView::requestConnectionToConnection(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint) {
	Q_EMIT connectionToConnectionRequested(_fromItemUid, _fromItemConnector, _toConnectionUid, _newControlPoint);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Callback handling

void ot::GraphicsView::notifyItemConfigurationChanged(const ot::GraphicsItem* _item) {
	// Avoid notification of child items
	if (_item != _item->getRootItem()) {
		return;
	}

	if (m_viewStateFlags & ItemMoveInProgress) {
		return;
	}

	// Ensure item is not a connector
	if (!_item->isInternalItem()) {
		GraphicsChangeEvent evt;
		evt.addChangedItem(_item->getConfiguration()->createCopy());

		for (const GraphicsConnectionItem* connection : _item->getAllConnections()) {
			evt.addChangedConnection(connection->getConfiguration());
		}

		Q_EMIT elementsChanged(evt);
	}
}

void ot::GraphicsView::notifyConnectionConfigurationChanged(const ot::GraphicsConnectionItem* _connection) {
	GraphicsChangeEvent evt;
	evt.addChangedConnection(_connection->getConfiguration());
	Q_EMIT elementsChanged(evt);
}

// ########################################################################################################

// Protected: Events

void ot::GraphicsView::wheelEvent(QWheelEvent* _event) {
	if ((m_viewStateFlags & MiddleMousePressedState) || !m_wheelEnabled) return;
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
			
	this->setTransformationAnchor(anchor);

	this->ensureViewInBounds();
}

void ot::GraphicsView::mousePressEvent(QMouseEvent* _event) {
	QGraphicsView::mousePressEvent(_event);

	if (_event->button() == Qt::MiddleButton) {
		this->viewport()->setCursor(Qt::ClosedHandCursor);
		m_lastPanPos = _event->pos();
		m_viewStateFlags |= MiddleMousePressedState;
	}
	else if (_event->button() == Qt::LeftButton) {
		this->beginItemMove();
	}
}

void ot::GraphicsView::mouseReleaseEvent(QMouseEvent* _event) {
	QGraphicsView::mouseReleaseEvent(_event);

	if (_event->button() == Qt::MiddleButton) {
		m_viewStateFlags &= (~MiddleMousePressedState);
		this->viewport()->setCursor(Qt::CrossCursor);
	}
	else if (_event->button() == Qt::LeftButton) {
		this->endItemMove();
	}
}

void ot::GraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
	if (m_viewStateFlags & MiddleMousePressedState) {
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
		ot::UIDList itm = this->getSelectedItemUIDs();
		ot::UIDList con = this->getSelectedConnectionUIDs();
		if (itm.empty() && con.empty()) return;
		Q_EMIT removeItemsRequested(itm, con);
	}
	else if (_event->key() == Qt::Key_Left) {
		if (_event->modifiers() & (Qt::ControlModifier | Qt::AltModifier)) {
			m_scene->flipAllSelectedItems(Qt::Horizontal);
		}
		else {
			this->beginItemMove();
			m_scene->moveAllSelectedItems(Point2DD(-m_scene->getGrid().getGridStep().x(), 0.));
		}
	}
	else if (_event->key() == Qt::Key_Right) {
		if (_event->modifiers() & (Qt::ControlModifier | Qt::AltModifier)) {
			m_scene->flipAllSelectedItems(Qt::Horizontal);
		}
		else {
			this->beginItemMove();
			m_scene->moveAllSelectedItems(Point2DD(m_scene->getGrid().getGridStep().x(), 0.));
		}
	}
	else if (_event->key() == Qt::Key_Up) {
		if (_event->modifiers() & (Qt::ControlModifier | Qt::AltModifier)) {
			m_scene->flipAllSelectedItems(Qt::Vertical);
		}
		else {
			this->beginItemMove();
			m_scene->moveAllSelectedItems(Point2DD(0., -m_scene->getGrid().getGridStep().y()));
		}
	}
	else if (_event->key() == Qt::Key_Down) {
		if (_event->modifiers() & (Qt::ControlModifier | Qt::AltModifier)) {
			m_scene->flipAllSelectedItems(Qt::Vertical);
		}
		else {
			this->beginItemMove();
			m_scene->moveAllSelectedItems(Point2DD(0., m_scene->getGrid().getGridStep().y()));
		}
	}
	else if (_event->key() == Qt::Key_R) {
		if (_event->modifiers() & Qt::ControlModifier) {
			m_scene->rotateAllSelectedItems(-90.);
		}
		else {
			m_scene->rotateAllSelectedItems(90.);
		}
	}
}

void ot::GraphicsView::keyReleaseEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Left && !_event->isAutoRepeat()) {
		if (!(_event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) {
			this->endItemMove();
		}
	}
	else if (_event->key() == Qt::Key_Right && !_event->isAutoRepeat()) {
		if (!(_event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) {
			this->endItemMove();
		}
	}
	else if (_event->key() == Qt::Key_Up && !_event->isAutoRepeat()) {
		if (!(_event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) {
			this->endItemMove();
		}
	}
	else if (_event->key() == Qt::Key_Down && !_event->isAutoRepeat()) {
		if (!(_event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) {
			this->endItemMove();
		}
	}
}

void ot::GraphicsView::resizeEvent(QResizeEvent* _event)
{
	QGraphicsView::resizeEvent(_event);

	this->ensureViewInBounds();
}

void ot::GraphicsView::dragEnterEvent(QDragEnterEvent* _event) {
	if (!m_dropEnabled) {
		_event->ignore();
		//QGraphicsView::dragEnterEvent(_event);
		return;
	}

	// Check item name provided
	if (_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName).isEmpty()) {
		_event->ignore();
		return;
	}

	// Check owner provided
	std::string pickerKey = _event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_PickerKey).toStdString();
	if (pickerKey.empty()) {
		_event->ignore();
		return;
	}

	// Check owner equals
	if (pickerKey != m_pickerKey) {
		_event->ignore();
		return;
	}

	_event->acceptProposedAction();
}

void ot::GraphicsView::dropEvent(QDropEvent* _event) {
	OTAssertNullptr(m_scene);

	if (!m_dropEnabled) {
		_event->ignore();
		return;
	}
	QString itemName = _event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName);
	if (itemName.isEmpty()) {
		OT_LOG_W("Drop event reqected: MimeData not matching");
		_event->ignore();
		return;
	}

	// Check owner provided
	std::string pickerKey = _event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_PickerKey).toStdString();
	if (pickerKey.empty()) {
		_event->ignore();
		return;
	}

	// Check owner equals
	if (pickerKey != m_pickerKey) {
		_event->ignore();
		return;
	}

	// Snap position to grid
	QPointF gridPos = m_scene->snapToGrid(this->mapToScene(_event->pos()));
	
	// Emit event
	Q_EMIT itemRequested(itemName, gridPos);
	_event->acceptProposedAction();
}

void ot::GraphicsView::dragMoveEvent(QDragMoveEvent* _event) {
	if (!m_dropEnabled) {
		_event->ignore();
		//QGraphicsView::dragEnterEvent(_event);
		return;
	}

	// Check item name provided
	if (_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName).isEmpty()) {
		_event->ignore();
		return;
	}

	// Check owner provided
	std::string pickerKey = _event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_PickerKey).toStdString();
	if (pickerKey.empty()) {
		_event->ignore();
		return;
	}

	// Check owner equals
	if (pickerKey != m_pickerKey) {
		_event->ignore();
		return;
	}

	_event->acceptProposedAction();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::GraphicsView::slotCopy() {
	CopyInformation info;
	if (this->getParentWidgetView()) {
		info.setOriginViewInfo(this->getParentWidgetView()->getViewData());
	}
	Q_EMIT copyRequested(info);
}

void ot::GraphicsView::slotPaste() {
	CopyInformation info;

	if (this->getParentWidgetView()) {
		info.setDestinationViewInfo(this->getParentWidgetView()->getViewData());
	}

	QPoint mousePos = this->mapFromGlobal(QCursor::pos());
	if (this->rect().contains(mousePos)) {
		// If mouse is over the view paste at cursor
		QPointF mouseScenePos = m_scene->snapToGrid(this->mapToScene(mousePos));
		info.setDestinationScenePos(QtFactory::toPoint2D(mouseScenePos));
	}

	Q_EMIT pasteRequested(info);
}

void ot::GraphicsView::slotGraphicsItemDoubleClicked(ot::GraphicsItem* _item) {
	Q_EMIT itemDoubleClicked(_item->getConfiguration());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::GraphicsView::beginItemMove() {
	if (m_viewStateFlags & (ItemMoveInProgress | ReadOnlyState)) {
		return;
	}

	m_viewStateFlags.set(ItemMoveInProgress, true);

	for (QGraphicsItem* qItm : m_scene->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(qItm);
		if (otItem) {
			otItem->setCurrentPosAsMoveStart();
		}
	}
}

void ot::GraphicsView::endItemMove() {
	if (!(m_viewStateFlags & ItemMoveInProgress) || (m_viewStateFlags & ReadOnlyState)) {
		return;
	}

	m_viewStateFlags.set(ItemMoveInProgress, false);

	// Notify about move
	GraphicsChangeEvent changeEvent;
	changeEvent.setEditorName(m_viewName);

	for (QGraphicsItem* qItm : m_scene->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(qItm);
		GraphicsConnectionItem* otConn = dynamic_cast<GraphicsConnectionItem*>(qItm);
		if (otItem) {
			if (otItem->isInternalItem()) {
				otItem->notifyConnectionsMove(changeEvent);
			}
			else {
				otItem->notifyMoveIfRequired(changeEvent);
			}
		}
	}

	for (auto& itm : m_items) {
		itm.second->checkConnectionSnapRequest(changeEvent);
	}

	if (!changeEvent.isEmpty()) {
		Q_EMIT elementsChanged(changeEvent);
	}
}
