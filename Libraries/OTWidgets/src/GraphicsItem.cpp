//! @file GraphicsItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/Positioning.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/ToolTipHandler.h"
#include "OTWidgets/GraphicsStackItem.h"
#include "OTWidgets/GraphicsHighlightItem.h"
#include "OTWidgets/GraphicsConnectionItem.h"

// Qt header
#include <QtCore/qmath.h>
#include <QtCore/qmimedata.h>
#include <QtGui/qdrag.h>
#include <QtGui/qfont.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtooltip.h>
#include <QtWidgets/qstyleoption.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicssceneevent.h>

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor / Destructor

ot::GraphicsItem::GraphicsItem(bool _isLayoutOrStack)
	: m_flags(GraphicsItemCfg::NoFlags), m_parent(nullptr), m_isLayoutOrStack(_isLayoutOrStack), 
	m_state(NoState), m_scene(nullptr), m_alignment(ot::AlignCenter), m_minSize(0., 0.), m_maxSize(DBL_MAX, DBL_MAX),
	m_sizePolicy(ot::Preferred), m_requestedSize(-1., -1.), m_connectionDirection(ot::ConnectAny), m_uid(0), m_highlightItem(nullptr), m_config(nullptr)
{

}

ot::GraphicsItem::~GraphicsItem() {

}

// ###############################################################################################################################################

// Virtual functions

bool ot::GraphicsItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	
	if (m_config != _cfg) {
		if (m_config) delete m_config;
		m_config = nullptr;
		m_config = _cfg->createCopy();
	}

	m_uid = _cfg->uid();
	m_toolTip = _cfg->toolTip();	
	m_alignment = _cfg->alignment();
	m_sizePolicy = _cfg->sizePolicy();
	m_margins = _cfg->margins();
	m_minSize = QSizeF(_cfg->minimumSize().width(), _cfg->minimumSize().height());
	m_maxSize = QSizeF(_cfg->maximumSize().width(), _cfg->maximumSize().height());
	m_moveStartPt = QPointF(_cfg->position().x(), _cfg->position().y());
	m_connectionDirection = _cfg->connectionDirection();

	this->setGraphicsItemName(_cfg->name());
	this->setGraphicsItemFlags(_cfg->graphicsItemFlags());

	return true;
}

ot::GraphicsItem* ot::GraphicsItem::findItem(const std::string& _itemName) {
	if (_itemName == m_name) return this;
	
	OTAssertNullptr(this->getQGraphicsItem());
	ot::GraphicsItem* ret = nullptr;

	for (auto c : this->getQGraphicsItem()->childItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(c);
		if (itm) {
			ret = itm->findItem(_itemName);
			if (ret) break;
		}
	}

	return ret;
}

void ot::GraphicsItem::removeAllConnections(void) {
	if (m_connections.empty()) return;

	GraphicsScene* scene = this->graphicsScene();
	if (scene == nullptr) {
		OT_LOG_EA("No graphics scene set");
		return;
	}
	GraphicsView* view = scene->getGraphicsView();
	OTAssertNullptr(view);

	std::list<ot::GraphicsConnectionCfg> graphicConnectionCfgList;
	for (const auto& connection : m_connections) {
		graphicConnectionCfgList.push_back(connection->getConnectionInformation());
	}
	for (const auto& connection : graphicConnectionCfgList) {
		view->removeConnection(connection);
	}
}

// ###############################################################################################################################################

// Event handler

void ot::GraphicsItem::handleMousePressEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_parent) {
		m_parent->handleMousePressEvent(_event);
	}
	else if (m_flags & GraphicsItemCfg::ItemIsConnectable) {
		OTAssertNullptr(m_scene);
		m_scene->startConnection(this);
	}
	else {
		OTAssertNullptr(m_scene);
		if (_event->modifiers() != Qt::ControlModifier) {
			m_scene->setIgnoreEvents(true);
			m_scene->clearSelection();
			m_scene->setIgnoreEvents(false);
		}
		this->getQGraphicsItem()->setSelected(true);
		this->handleItemChange(QGraphicsItem::ItemSelectedHasChanged, QVariant());

		m_scene->handleSelectionChanged();

		if ((m_flags & GraphicsItemCfg::ItemIsMoveable) && _event->modifiers() != Qt::ControlModifier) {
			auto qitm = this->getQGraphicsItem();
			OTAssertNullptr(qitm);
			m_moveStartPt = qitm->pos(); // The item is root item, so pos returns the scene pos
		}
	}
}

void ot::GraphicsItem::handleMouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_parent) {
		m_parent->handleMousePressEvent(_event);
	}
	else if ((m_flags & GraphicsItemCfg::ItemIsMoveable) && _event->modifiers() != Qt::ControlModifier) {
		auto qitm = this->getQGraphicsItem();
		OTAssertNullptr(qitm);
		// Check if the item has moved after the user released the mouse
		if (qitm->pos() != m_moveStartPt) {
			OTAssertNullptr(m_scene);
			m_scene->getGraphicsView()->notifyItemMoved(this);
		}
	}
}

void ot::GraphicsItem::handleHoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleToolTip(_event);
	if (m_flags & GraphicsItemCfg::ItemHasNoFeedback) {
		return;
	}

	this->m_state |= GraphicsItem::HoverState;
	this->getQGraphicsItem()->update();
	if (m_highlightItem) {
		m_highlightItem->handleHoverEnterEvent(_event);
	}
}

void ot::GraphicsItem::handleToolTip(QGraphicsSceneHoverEvent* _event) {
	if (!m_toolTip.empty())
	{
		ToolTipHandler::showToolTip(_event->screenPos(), QString::fromStdString(m_toolTip), 1500);
	}
	else if ((m_flags & GraphicsItemCfg::ItemForwardsTooltip) && m_parent) {
		m_parent->handleToolTip(_event);
	}
}

void ot::GraphicsItem::handleHoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	ToolTipHandler::hideToolTip();
	this->m_state &= (~GraphicsItem::HoverState);
	this->getQGraphicsItem()->update();
	if (m_highlightItem) {
		m_highlightItem->handleHoverLeaveEvent(_event);
	}
}

void ot::GraphicsItem::paintStateBackground(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (m_flags & GraphicsItemCfg::ItemHasNoFeedback) return;

	if (m_state & HoverState) {
		QPen p(QColor(0, 0, 255));
		_painter->setPen(p);
		_painter->fillRect(this->getQGraphicsItem()->boundingRect(), QColor(0, 0, 255));
	}
	else if (m_state & SelectedState) {
		QPen p(QColor(0, 255, 0));
		_painter->setPen(p);
		_painter->fillRect(this->getQGraphicsItem()->boundingRect(), QColor(0, 255, 0));
	}
}

QSizeF ot::GraphicsItem::handleGetGraphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _sizeHint) const {
	// The adjusted size is the size hint expanded to the minimum size, bound to maximum size and with margins applied
	QSizeF adjustedSize = _sizeHint.expandedTo(m_minSize).boundedTo(m_maxSize);

	switch (_hint)
	{
	case Qt::MinimumSize:
		return this->applyGraphicsItemMargins(m_minSize);
	case Qt::PreferredSize:
	case Qt::MinimumDescent:
	case Qt::NSizeHints:
		break;
	case Qt::MaximumSize:
		return m_maxSize;
	default:
		OT_LOG_WA("Unknown Qt SizeHint");
	}
	return this->applyGraphicsItemMargins(adjustedSize.boundedTo(m_maxSize));
}

QRectF ot::GraphicsItem::handleGetGraphicsItemBoundingRect(const QRectF& _rect) const {
	if (m_sizePolicy == ot::Dynamic) {
		return QRectF(
			_rect.topLeft(),
			this->applyGraphicsItemMargins(m_minSize.expandedTo(this->removeGraphicsItemMargins(m_requestedSize)).boundedTo(m_maxSize))
		);
	}
	else {
		return QRectF(
			_rect.topLeft(),
			this->applyGraphicsItemMargins(_rect.size().expandedTo(m_minSize).expandedTo(this->removeGraphicsItemMargins(m_requestedSize)).boundedTo(m_maxSize))
		);
	}
	
}

void ot::GraphicsItem::handleItemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	switch (_change)
	{
	case QGraphicsItem::ItemSelectedHasChanged:
		if (this->getQGraphicsItem()->isSelected() && !(m_state & SelectedState)) {
			m_state |= SelectedState;
			if (m_highlightItem) {
				m_highlightItem->setStateFlags(m_highlightItem->stateFlags() | SelectedState);
				m_highlightItem->update();
			}
		}
		else if (!this->getQGraphicsItem()->isSelected() && (m_state & SelectedState)) {
			m_state &= (~SelectedState);
			if (m_highlightItem) {
				m_highlightItem->setStateFlags(m_highlightItem->stateFlags() & (~SelectedState));
				m_highlightItem->update();
			}
		}
		break;
	case QGraphicsItem::ItemScenePositionHasChanged:
		for (auto c : m_connections) {
			c->updateConnection();
		}
		this->raiseEvent(ot::GraphicsItem::ItemMoved);
		break;
	default:
		break;
	}
}

void ot::GraphicsItem::handleSetItemGeometry(const QRectF& _geom) {
	if (m_parent) {
		if (m_parent->isLayoutOrStack()) {
			this->setGraphicsItemRequestedSize(_geom.size());
		}
	}
}

// ###############################################################################################################################################

// Getter / Setter

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItemCfg::GraphicsItemFlags _flags) {
	m_flags = _flags;
	this->graphicsItemFlagsChanged(m_flags);
}

ot::GraphicsScene* ot::GraphicsItem::graphicsScene(void) {
	if (m_parent) {
		return m_parent->graphicsScene();
	}
	else {
		return m_scene;
	}
}

ot::GraphicsItem* ot::GraphicsItem::getRootItem(void) {
	if (m_parent) {
		return m_parent->getRootItem();
	}
	else {
		return this;
	}
}

void ot::GraphicsItem::storeConnection(GraphicsConnectionItem* _connection) {
	m_connections.push_back(_connection);
}

void ot::GraphicsItem::forgetConnection(GraphicsConnectionItem* _connection) {
	auto connection = std::find(m_connections.begin(), m_connections.end(), _connection);
	while (connection != m_connections.end()) {
		m_connections.erase(connection);
		connection = std::find(m_connections.begin(), m_connections.end(), _connection);
	}
}

void ot::GraphicsItem::addGraphicsItemEventHandler(ot::GraphicsItem* _handler) {
	m_eventHandler.push_back(_handler);
}

void ot::GraphicsItem::removeGraphicsItemEventHandler(ot::GraphicsItem* _handler) {
	auto it = std::find(m_eventHandler.begin(), m_eventHandler.end(), _handler);
	while (it != m_eventHandler.end()) {
		m_eventHandler.erase(it);
		it = std::find(m_eventHandler.begin(), m_eventHandler.end(), _handler);
	}
}

void ot::GraphicsItem::setGraphicsItemRequestedSize(const QSizeF& _size) {
	m_requestedSize = _size;
	this->prepareGraphicsItemGeometryChange();
}

void ot::GraphicsItem::raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event) {
	for (auto itm : m_eventHandler) {
		itm->graphicsItemEventHandler(this, _event);
	}
}

QSizeF ot::GraphicsItem::applyGraphicsItemMargins(const QSizeF& _size) const {
	return QSizeF(_size.width() + m_margins.left() + m_margins.right(), _size.height() + m_margins.top() + m_margins.bottom());
}

QSizeF ot::GraphicsItem::removeGraphicsItemMargins(const QSizeF& _size) const {
	return QSizeF((_size.width() - m_margins.left()) - m_margins.right(), (_size.height() - m_margins.top()) - m_margins.bottom());
}

QRectF ot::GraphicsItem::calculatePaintArea(const QSizeF& _innerSize) {
	auto qitm = this->getQGraphicsItem();
	OTAssertNullptr(qitm);
	
	// Get the bounding rect of the item
	QRectF r(qitm->boundingRect());

	// Adjust size
	QSizeF inner;

	switch (m_sizePolicy)
	{
	case ot::Preferred:
		inner = _innerSize;
		break;
	case ot::Dynamic:
		inner = m_minSize;
		break;
	default:
		break;
	}

	// Calculate the size of the inner rectangle
	inner = inner.expandedTo(m_minSize).expandedTo(this->removeGraphicsItemMargins(m_requestedSize)).boundedTo(m_maxSize);

	if (inner.toSize() == r.size()) {
		// No further adjustments needed
		return r;
	}
	else {
		// Calculate the inner rectangle
		return ot::calculateChildRect(r, inner, m_alignment);
	}
}

std::list<ot::GraphicsConnectionCfg> ot::GraphicsItem::getConnectionCfgs() 
{
	std::list<ot::GraphicsConnectionCfg> graphicConnectionCfgList;
	for (const auto& connection : m_connections) 
	{
		graphicConnectionCfgList.push_back(connection->getConnectionInformation());
	}

	for (auto childQGraphicsItem : getQGraphicsItem()->childItems()) 
	{
		ot::GraphicsItem* graphicsItem= dynamic_cast<ot::GraphicsItem*>(childQGraphicsItem);
		if (graphicsItem != nullptr)
		{
			graphicConnectionCfgList.splice(graphicConnectionCfgList.end(),graphicsItem->getConnectionCfgs());
		}
	}
	return graphicConnectionCfgList;
}

void ot::GraphicsItem::createHighlightItem(void) {
	GraphicsHighlightItem* newItem = new GraphicsHighlightItem;
	newItem->setGraphicsItemFlags(ot::GraphicsItemCfg::NoFlags);
	this->setHighlightItem(newItem);
}

void ot::GraphicsItem::setHighlightItem(GraphicsHighlightItem* _item) {
	if (m_highlightItem) delete m_highlightItem;
	m_highlightItem = _item;
}
