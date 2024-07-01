//! @file GraphicsView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsItemPreviewDrag.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtCore/qmimedata.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qgraphicsproxywidget.h>

//! \brief If defined as true the graphics connection API related code will generate more detailed log messages.
//! The messages will contain creation, deletion and other detailed informations about the objects lifetime.
//! \warning Never use in deployment!
#define OT_DBG_WIDGETS_GRAPHICS_VIEW_API false

#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#pragma message("! ot: GraphicsView debug is enabled.                  !")
#pragma message("! ot:NoDeploy: Do not use this build in a deployment. !")
#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#endif

ot::GraphicsView::GraphicsView(GraphicsScene* _scene) 
	: m_scene(_scene), m_wheelEnabled(true), m_dropEnabled(false),
	m_viewFlags(NoViewFlags), m_viewStateFlags(DefaultState)
{
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview creating 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
	if (!m_scene) m_scene = new GraphicsScene(this);

	this->setScene(m_scene);
	this->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
	this->setAlignment(Qt::AlignAbsolute);

	this->setUpdatesEnabled(true);
}

ot::GraphicsView::~GraphicsView() {
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview destroying 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
}

// ########################################################################################################

void ot::GraphicsView::resetView(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = s->itemsBoundingRect();
	if (m_viewFlags & ViewManagesSceneRect) {
		this->setSceneRect(m_scene->itemsBoundingRect().marginsAdded(QMargins(100, 100, 100, 100)));
	}
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
	if (m_viewFlags & ViewManagesSceneRect) {
		this->setSceneRect(m_scene->itemsBoundingRect().marginsAdded(QMargins(100, 100, 100, 100)));
	}
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
		//resetView();
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
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview adding item { "
		"\"this\": \"0x" + ot::numberToHexString<size_t>((size_t)this) +
		"\", \"item\": \"0x" + ot::numberToHexString<size_t>((size_t)_item) + "\" }"
	);
#endif

	m_viewStateFlags |= ItemAddInProgress;

	auto it = m_items.find(_item->getGraphicsItemUid());
	bool removeConnectionBufferApplied = false;
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

	if (m_scene->getGrid().isGridSnapValid()) {
		_item->setGraphicsItemPos(m_scene->snapToGrid(_item->getGraphicsItemPos()));
	}

	_item->setBlockConfigurationNotifications(false);

	if (removeConnectionBufferApplied) {
		for (const GraphicsConnectionCfg& bufferedConnection : m_itemRemovalConnectionBuffer) {
			this->addConnection(bufferedConnection);
		}
		m_itemRemovalConnectionBuffer.clear();
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
	m_viewStateFlags &= ~(ItemAddInProgress);
}

void ot::GraphicsView::removeItem(const ot::UID& _itemUid, bool bufferConnections) {
	auto graphicsItemByUID = m_items.find(_itemUid);
	if (graphicsItemByUID == m_items.end()) {
		//OT_LOG_EAS("Item with the ID \"" + _itemUid + "\" could not be found");
		return;
	}

	m_viewStateFlags |= ItemRemoveInProgress;

	this->blockSignals(true);
	m_scene->blockSignals(true);

	ot::GraphicsItem* graphicsItem =  graphicsItemByUID->second;

#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview removing item { "
		"\"this\": \"0x" + ot::numberToHexString<size_t>((size_t)this) +
		"\", \"item\": \"0x" + ot::numberToHexString<size_t>((size_t)graphicsItem) + "\" }"
	);
#endif

	OTAssertNullptr(graphicsItem);
	OTAssertNullptr(graphicsItem->getQGraphicsItem());
	graphicsItem->setStateFlag(ot::GraphicsItem::ToBeDeletedState, true);

	m_items.erase(_itemUid);

	if (bufferConnections)
	{
		m_itemRemovalConnectionBuffer = graphicsItem->getConnectionCfgs();
		m_itemRemovalConnectionBuffer.unique();
	}
	graphicsItem->removeAllConnections();

	graphicsItem->setGraphicsScene(nullptr);
	m_scene->removeItem(graphicsItem->getQGraphicsItem());

	// Re-enable block signals avter removeAllConnections
	this->blockSignals(true);
	m_scene->blockSignals(true);

	delete graphicsItem;
	
	m_scene->blockSignals(false);
	this->blockSignals(false);

	m_viewStateFlags &= ~(ItemRemoveInProgress);
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
	if (this->connectedGraphicItemsExist(_config))
	{
		this->addConnection(_config);
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
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview adding connection { "
		"\"this\": \"0x" + ot::numberToHexString<size_t>((size_t)this) +
		"\", \"uid\": " + std::to_string(_config.getUid()) + " }"
	);
#endif

	m_viewStateFlags |= ConnectionAddInProgress;

	this->removeConnection(_config.getUid());
	ot::GraphicsItem* src = this->getItem(_config.getOriginUid());
	ot::GraphicsItem* dest = this->getItem(_config.getDestinationUid());

	if (!src || !dest) {
		OT_LOG_EA("Invalid item UID");
		m_viewStateFlags &= ~(ConnectionAddInProgress);
		return;
	}

	ot::GraphicsItem* srcConn = src->findItem(_config.getOriginConnectable());
	ot::GraphicsItem* destConn = dest->findItem(_config.getDestConnectable());

	if (!srcConn || !destConn) {
		OT_LOG_EA("Invalid connectable name");
		m_viewStateFlags &= ~(ConnectionAddInProgress);
		return;
	}

	// Create and add new connection
	ot::GraphicsConnectionItem* newConnection = new ot::GraphicsConnectionItem;
	newConnection->setConfiguration(_config);
	
	m_scene->addItem(newConnection);
	newConnection->connectItems(srcConn, destConn);
	newConnection->setZValue(1);

	m_connections.insert_or_assign(_config.getUid(), newConnection);

#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview added connection { "
		"\"this\": \"0x" + ot::numberToHexString<size_t>((size_t)this) +
		"\", \"connection\": \"0x" + ot::numberToHexString<size_t>((size_t)newConnection) + "\" }"
	);
#endif

	m_viewStateFlags &= ~(ConnectionAddInProgress);
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
		//OT_LOG_W("Connection not found { \"UID\": " + std::to_string(_connectionUID) + " }");
		return;
	}

	// Remove connection from items
	ot::GraphicsConnectionItem* connection = connectionByUID->second;
	if (!connection) {
		OT_LOG_EA("[FATAL] null stored");
		return;
	}
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview removing connection { "
		"\"this\": \"0x" + ot::numberToHexString<size_t>((size_t)this) +
		"\", \"connection\": \"0x" + ot::numberToHexString<size_t>((size_t)connection) + "\" }"
	);
#endif

	// Call remove connection for the actual item to remove it from the scene and maps
	this->removeConnection(connection);

	m_viewStateFlags |= ConnectionRemoveInProgress;

	// Destroy connection
	delete connection;
	connection = nullptr;

	m_viewStateFlags &= ~(ConnectionRemoveInProgress);
}

void ot::GraphicsView::removeConnection(GraphicsConnectionItem* _item) {
	OTAssertNullptr(_item);

	this->blockSignals(true);
	m_scene->blockSignals(true);

	m_viewStateFlags |= ConnectionRemoveInProgress;
	_item->setStateFlags(GraphicsConnectionItem::ToBeDeletedState);

	UID uid = 0;
	if (_item) {
		uid = _item->getConfiguration().getUid();

		// Let connected items forget this connection
		_item->disconnectItems();

		// Remove from scene
		m_scene->removeItem(_item);
	}

	// Erase connection from map
	m_connections.erase(uid);

	m_viewStateFlags &= ~(ConnectionRemoveInProgress);

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
	if (m_viewStateFlags & AnyChangeInProgress) return;
	if (this->connectionAlreadyExists(ot::GraphicsConnectionCfg(_fromUid, _fromConnector, _toUid, _toConnector))) {
		OT_LOG_W("Connection already exists { \"Origin.UID\": \"" + std::to_string(_fromUid) + "\", \"Origin.Conn\"" + _fromConnector + "\", \"Dest.UID\": \"" + std::to_string(_toUid) + "\", \"Dest.Conn\": \"" + _toConnector + "\" }");
		return;
	}
	Q_EMIT connectionRequested(_fromUid, _fromConnector, _toUid, _toConnector);
}

void ot::GraphicsView::notifyItemMoved(const ot::GraphicsItem* _item) {
	if (m_viewStateFlags & AnyChangeInProgress) return;
	Q_EMIT itemMoved(_item->getGraphicsItemUid(), QtFactory::toQPoint(_item->getGraphicsItemPos()));
}

void ot::GraphicsView::notifyItemConfigurationChanged(const ot::GraphicsItem* _item) {
	// Avoid notification of child items
	if (_item != _item->getRootItem()) return;

	if (m_viewStateFlags & AnyChangeInProgress) return;
	Q_EMIT itemConfigurationChanged(_item->getConfiguration());
}

// ########################################################################################################

// Protected: Event handler

bool ot::GraphicsView::event(QEvent* _event) {
	if (m_viewStateFlags & ViewStateFlag::AnyChangeInProgress) {
		_event->setAccepted(true);
		return true;
	}
	else {
		return QGraphicsView::event(_event);
	}
}

void ot::GraphicsView::wheelEvent(QWheelEvent* _event)
{
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview wheel event 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
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
	//this->update();
		
	this->setTransformationAnchor(anchor);

	this->viewAll();
}

void ot::GraphicsView::mousePressEvent(QMouseEvent* _event)
{
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview mouse press 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
	QGraphicsView::mousePressEvent(_event);
	if (_event->button() == Qt::MiddleButton) {
		this->viewport()->setCursor(Qt::ClosedHandCursor);
		m_lastPanPos = _event->pos();
		m_viewStateFlags |= MiddleMousePressedState;
	}
}

void ot::GraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview mouse release 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
	QGraphicsView::mouseReleaseEvent(_event);

	if (_event->button() == Qt::MiddleButton) {
		m_viewStateFlags &= (~MiddleMousePressedState);
		this->viewport()->setCursor(Qt::CrossCursor);
	}
}

void ot::GraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview mouse move 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
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
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview key press 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
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
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview key release 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
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
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview resize 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
	QGraphicsView::resizeEvent(_event);

	this->viewAll();
}

void ot::GraphicsView::dragEnterEvent(QDragEnterEvent* _event) {
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview drag enter 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName).isEmpty() && m_dropEnabled) {
		_event->acceptProposedAction();
	}
	else {
		QGraphicsView::dragEnterEvent(_event);
	}
}

void ot::GraphicsView::dropEvent(QDropEvent* _event) {
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview drop event 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
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
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview drag move 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName).isEmpty() && m_dropEnabled) {
		_event->acceptProposedAction();
	}
	else {
		QGraphicsView::dragMoveEvent(_event);
	}
}

void ot::GraphicsView::beginItemMove(void) {
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview begin item move called 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
	if (m_viewStateFlags & ItemMoveInProgress) return;

	m_viewStateFlags.setFlag(ItemMoveInProgress, true);

	for (QGraphicsItem* qItm : m_scene->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(qItm);
		if (otItem) {
			otItem->setCurrentPosAsMoveStart();
		}
	}

#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview begin item move completed 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
}

void ot::GraphicsView::endItemMove(void) {
#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview end item move called 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif

	if (!(m_viewStateFlags & ItemMoveInProgress)) return;

	m_viewStateFlags.setFlag(ItemMoveInProgress, false);

	for (QGraphicsItem* qItm : m_scene->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(qItm);
		if (otItem) {
			otItem->notifyMoveIfRequired();
		}
	}

#if OT_DBG_WIDGETS_GRAPHICS_VIEW_API==true
	OT_LOG_D("debug.graphicsview end item move completed 0x" + ot::numberToHexString<size_t>((size_t)this));
#endif
}
