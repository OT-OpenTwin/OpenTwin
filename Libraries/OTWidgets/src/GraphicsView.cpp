//! @file GraphicsView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/CopyInformationFactory.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsCopyInformation.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsItemPreviewDrag.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qclipboard.h>
#include <QtCore/qmimedata.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qgraphicsproxywidget.h>

ot::GraphicsView::GraphicsView(GraphicsScene* _scene) 
	: m_scene(_scene), m_wheelEnabled(true), m_dropEnabled(false),
	m_viewFlags(NoViewFlags), m_viewStateFlags(DefaultState), m_sceneMargins(5., 5., 5., 5.)
{
	if (!m_scene) m_scene = new GraphicsScene(this);

	this->setScene(m_scene);
	this->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
	this->setAlignment(Qt::AlignAbsolute);

	this->setUpdatesEnabled(true);

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

// ########################################################################################################

void ot::GraphicsView::resetView(void) {
	OTAssertNullptr(m_scene);
	QRectF boundingRect = m_scene->itemsBoundingRect().marginsAdded(m_sceneMargins);
	if (m_viewFlags & ViewManagesSceneRect) {
		this->setSceneRect(boundingRect.marginsAdded(m_sceneMargins));
	}

	this->fitInView(boundingRect, Qt::AspectRatioMode::KeepAspectRatio);
	this->centerOn(boundingRect.center());
}

void ot::GraphicsView::ensureViewInBounds(void) {
	OTAssertNullptr(m_scene);
	QRect itemsRect = this->mapFromScene(m_scene->itemsBoundingRect().marginsAdded(m_sceneMargins)).boundingRect();
	QRect viewPortRect = this->viewport()->rect();

	if (itemsRect.width() < viewPortRect.width() && itemsRect.height() < viewPortRect.height()) {
		this->resetView();
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
		if (connection.second->getConfiguration().createConnectionKey() == _connection.createConnectionKey() || connection.second->getConfiguration().createConnectionKey() == _connection.createConnectionKeyReverse())
		{
			alreadyExisting = true;
			break;
		}
	}
	return (alreadyExisting);
}

void ot::GraphicsView::addItem(ot::GraphicsItem* _item) {
	auto it = m_items.find(_item->getGraphicsItemUid());
	bool removeConnectionBufferApplied = !m_itemRemovalConnectionBuffer.empty();
	if (it != m_items.end()) {
		OT_LOG_D("Overwriting item with the ID \"" + std::to_string(_item->getGraphicsItemUid()));
		removeConnectionBufferApplied = true;
		this->removeItem(_item->getGraphicsItemUid(), removeConnectionBufferApplied);
	}

	_item->setBlockConfigurationNotifications(true);

	m_items.insert_or_assign(_item->getGraphicsItemUid(), _item);
	m_scene->addItem(_item->getRootItem()->getQGraphicsItem());
	_item->getRootItem()->getQGraphicsItem()->setZValue(2);
	_item->setGraphicsScene(m_scene);

	QPointF pt = m_scene->snapToGrid(_item);
	if (pt != _item->getQGraphicsItem()->pos()) {
		_item->setGraphicsItemPos(pt);
	}

	// Check for max trigger distance
	m_scene->checkMaxTriggerDistance(_item->getMaxAdditionalTriggerDistance());

	_item->setBlockConfigurationNotifications(false);

	OT_LOG_D("Item added { \"UID\": " + std::to_string(_item->getGraphicsItemUid()) + " }");

	if (removeConnectionBufferApplied) {
		std::list<GraphicsConnectionCfg> newBuffer;
		for (const GraphicsConnectionCfg& bufferedConnection : m_itemRemovalConnectionBuffer) {
			if (!this->addConnectionIfConnectedItemsExist(bufferedConnection)) {
				newBuffer.push_back(bufferedConnection);
			}
		}
		m_itemRemovalConnectionBuffer = newBuffer;
	}

	auto currentConnection = m_connectionCreationBuffer.begin();
	while (currentConnection !=  m_connectionCreationBuffer.end()) {
		const bool connectionCreated = addConnectionIfConnectedItemsExist(*currentConnection);
		if (connectionCreated) {
			currentConnection =	m_connectionCreationBuffer.erase(currentConnection);
		}
		else {
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
	
	this->blockSignals(true);
	m_scene->blockSignals(true);

	ot::GraphicsItem* graphicsItem =  graphicsItemByUID->second;
	OTAssertNullptr(graphicsItem);
	if (bufferConnections)
	{
		m_itemRemovalConnectionBuffer = graphicsItem->getConnectionCfgs();
		m_itemRemovalConnectionBuffer.unique();
	}

	m_scene->itemAboutToBeRemoved(graphicsItem);
	graphicsItem->removeAllConnections();
	m_scene->removeItem(graphicsItem->getQGraphicsItem());
	graphicsItem->setGraphicsScene(nullptr);
	delete graphicsItem;
	graphicsItem = nullptr;
	m_items.erase(_itemUid);
	
	OT_LOG_D("Item removed { \"UID\": " + std::to_string(_itemUid) + " }");

	m_scene->blockSignals(false);
	this->blockSignals(false);
}

std::list<ot::UID> ot::GraphicsView::getSelectedItemUIDs(void) const {
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

std::list<ot::GraphicsItem*> ot::GraphicsView::getSelectedGraphicsItems(void) const {
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

bool ot::GraphicsView::addConnectionIfConnectedItemsExist(const GraphicsConnectionCfg& _config)
{
	if (connectedGraphicItemsExist(_config)) {
		addConnection(_config);
		return true;
	}
	else {
		if (std::find(m_connectionCreationBuffer.begin(), m_connectionCreationBuffer.end(), _config) == m_connectionCreationBuffer.end()) {
			m_connectionCreationBuffer.push_back(_config);
		}
		return false;
	}
}

void ot::GraphicsView::addConnection(const GraphicsConnectionCfg& _config) {
	this->removeConnection(_config.getUid());
	ot::GraphicsItem* src = this->getItem(_config.getOriginUid());
	ot::GraphicsItem* dest = this->getItem(_config.getDestinationUid());
	if (!src) {
		OT_LOG_EAS("Connection source not found { \"UID\": " + std::to_string(_config.getOriginUid()) + " }");
		return;
	}
	if (!dest) {
		OT_LOG_EAS("Connection destination not found { \"UID\": " + std::to_string(_config.getDestinationUid()) + " }");
		return;
	}
	ot::GraphicsItem* srcConn = src->findItem(_config.getOriginConnectable());
	ot::GraphicsItem* destConn = dest->findItem(_config.getDestConnectable());

	if (!srcConn || !destConn) {
		OT_LOG_EA("Invalid connectable name");
		return;
	}

	// Create and add new connection
	ot::GraphicsConnectionItem* newConnection = new ot::GraphicsConnectionItem;
	newConnection->setConfiguration(_config);
	
	m_scene->addItem(newConnection);
	newConnection->connectItems(srcConn, destConn);
	newConnection->setZValue(1);

	m_connections.insert_or_assign(_config.getUid(), newConnection);

	OT_LOG_D("New connection added { \"UID\": " + std::to_string(_config.getUid()) + " }");
}

bool ot::GraphicsView::connectedGraphicItemsExist(const GraphicsConnectionCfg& _config) {
	ot::GraphicsItem* src = this->getItem(_config.getOriginUid());
	ot::GraphicsItem* dest = this->getItem(_config.getDestinationUid());

	return src != nullptr && dest != nullptr;
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
	connection->disconnectItems();

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

ot::UIDList ot::GraphicsView::getSelectedConnectionUIDs(void) const {
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

std::list<ot::GraphicsConnectionItem*> ot::GraphicsView::getSelectedConnectionItems(void) const {
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

void ot::GraphicsView::notifyItemMoved(const ot::GraphicsItem* _item) {
	if (m_viewStateFlags & ItemMoveInProgress) return;
	Q_EMIT itemMoved(_item->getGraphicsItemUid(), QtFactory::toQPoint(_item->getGraphicsItemPos()));
}

void ot::GraphicsView::notifyItemConfigurationChanged(const ot::GraphicsItem* _item) {
	// Avoid notification of child items
	if (_item != _item->getRootItem()) {
		return;
	}

	if (m_viewStateFlags & ItemMoveInProgress) {
		return;
	}
	Q_EMIT itemConfigurationChanged(_item->getConfiguration());
}

QRectF ot::GraphicsView::getVisibleSceneRect(void) const {
	return this->mapToScene(this->viewport()->rect()).boundingRect();
}

// ########################################################################################################

// Protected: Slots

void ot::GraphicsView::wheelEvent(QWheelEvent* _event)
{
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
	if (_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_Owner).isEmpty()) {
		_event->ignore();
		return;
	}

	std::string ownerData = _event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_Owner).toStdString();
	if (ownerData.empty()) {
		_event->ignore();
		return;
	}

	JsonDocument doc;
	if (!doc.fromJson(ownerData)) {
		OT_LOG_E("Failed to parse owner document");
		_event->ignore();
		return;
	}
	BasicServiceInformation ownerInfo;
	ownerInfo.setFromJsonObject(doc.GetConstObject());

	// Check owner equals
	if (ownerInfo != m_owner) {
		_event->ignore();
		return;
	}

	_event->acceptProposedAction();
}

void ot::GraphicsView::dropEvent(QDropEvent* _event) {
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
	if (_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_Owner).isEmpty()) {
		_event->ignore();
		return;
	}

	std::string ownerData = _event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_Owner).toStdString();
	if (ownerData.empty()) {
		_event->ignore();
		return;
	}

	JsonDocument doc;
	if (!doc.fromJson(ownerData)) {
		OT_LOG_E("Failed to parse owner document");
		_event->ignore();
		return;
	}
	BasicServiceInformation ownerInfo;
	ownerInfo.setFromJsonObject(doc.GetConstObject());

	// Check owner equals
	if (ownerInfo != m_owner) {
		_event->ignore();
		return;
	}

	Q_EMIT itemRequested(itemName, this->mapToScene(_event->pos()));
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
	if (_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_Owner).isEmpty()) {
		_event->ignore();
		return;
	}

	std::string ownerData = _event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_Owner).toStdString();
	if (ownerData.empty()) {
		_event->ignore();
		return;
	}

	JsonDocument doc;
	if (!doc.fromJson(ownerData)) {
		OT_LOG_E("Failed to parse owner document");
		_event->ignore();
		return;
	}
	BasicServiceInformation ownerInfo;
	ownerInfo.setFromJsonObject(doc.GetConstObject());

	// Check owner equals
	if (ownerInfo != m_owner) {
		_event->ignore();
		return;
	}

	_event->acceptProposedAction();
}

void ot::GraphicsView::slotCopy(void) {
	QClipboard* clip = QApplication::clipboard();
	if (!clip) {
		OT_LOG_E("No clipboard found");
		return;
	}

	std::list<GraphicsItem*> selection = this->getSelectedGraphicsItems();
	if (selection.empty()) {
		return;
	}

	GraphicsCopyInformation copyInfo;
	copyInfo.setViewOwner(this->getOwner());

	for (GraphicsItem* itm : selection) {
		// Copy top level items only
		if (!itm->getParentGraphicsItem()) {
			copyInfo.addItemInformation(itm->getGraphicsItemUid(), itm->getGraphicsItemPos());
		}
	}

	JsonDocument exportDoc;
	copyInfo.addToJsonObject(exportDoc, exportDoc.GetAllocator());

	clip->setText(QString::fromStdString(exportDoc.toJson()));
}

void ot::GraphicsView::slotPaste(void) {
	QClipboard* clip = QApplication::clipboard();
	if (!clip) {
		OT_LOG_E("No clipboard found");
		return;
	}

	// Get data
	std::string importString = clip->text().toStdString();
	if (importString.empty()) {
		return;
	}

	JsonDocument importDoc;
	if (!importDoc.fromJson(importString)) {
		return;
	}
	if (!importDoc.IsObject()) {
		return;
	}

	GraphicsCopyInformation* info = dynamic_cast<GraphicsCopyInformation*>(CopyInformationFactory::create(importDoc.GetConstObject()));
	if (!info) {
		return;
	}

	// Ensure same view owner
	if (info->getViewOwner() != m_owner) {
		return;
	}

	// Create paste info
	info->setViewName(m_viewName);
	
	QPoint mousePos = this->mapFromGlobal(QCursor::pos());
	if (this->rect().contains(mousePos)) {
		// If mouse is over the view paste at cursor
		QPointF mouseScenePos = m_scene->snapToGrid(this->mapToScene(mousePos));
		info->moveItemsToPoint(QtFactory::toPoint2D(mouseScenePos));
	}
	else {
		// If mouse is outside the view move by 2*grid size or 20 in X and Y directions.
		Point2DD moveDistance(20., 20.);

		const Grid& grid = m_scene->getGrid();
		if (grid.getGridStep().x() > 0) {
			moveDistance.setX(grid.getGridStep().x() * 2.);
		}
		if (grid.getGridStep().y() > 0) {
			moveDistance.setY(grid.getGridStep().y() * 2.);
		}

		info->moveItemsBy(moveDistance);
	}
	
	Q_EMIT itemCopyRequested(info);

	delete info;
}

void ot::GraphicsView::beginItemMove(void) {
	if (m_viewStateFlags & (ItemMoveInProgress | ReadOnlyState)) {
		return;
	}

	m_viewStateFlags.setFlag(ItemMoveInProgress, true);

	for (QGraphicsItem* qItm : m_scene->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(qItm);
		if (otItem) {
			otItem->setCurrentPosAsMoveStart();
		}
	}
}

void ot::GraphicsView::endItemMove(void) {
	if (!(m_viewStateFlags & ItemMoveInProgress) || (m_viewStateFlags & ReadOnlyState)) {
		return;
	}

	m_viewStateFlags.setFlag(ItemMoveInProgress, false);

	for (QGraphicsItem* qItm : m_scene->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(qItm);
		if (otItem) {
			otItem->notifyMoveIfRequired();
		}
	}
}
