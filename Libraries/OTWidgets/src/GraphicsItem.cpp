//! @file GraphicsItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/QtFactory.h"
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

ot::GraphicsItem::GraphicsItem(GraphicsItemCfg* _configuration, const ot::Flags<GraphicsItemState>& _stateFlags)
	: m_config(_configuration), m_state(_stateFlags), m_highlightItem(nullptr), m_moveStartPt(0., 0.), m_parent(nullptr), m_scene(nullptr), m_requestedSize(-1., -1.)
{

}

ot::GraphicsItem::~GraphicsItem() {

}

// ###############################################################################################################################################

// Virtual functions

bool ot::GraphicsItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	
	if (m_config != _cfg) {
		if (m_config) delete m_config;
		m_config = nullptr;
		m_config = _cfg->createCopy();
	}
	
	if (m_config) {
		m_moveStartPt = QPointF(m_config->getPosition().x(), m_config->getPosition().y());
		this->setGraphicsItemName(this->getGraphicsItemName());
		this->graphicsItemFlagsChanged(this->getGraphicsItemFlags());
	}
	
	return true;
}

ot::GraphicsItem* ot::GraphicsItem::findItem(const std::string& _itemName) {
	if (_itemName == this->getGraphicsItemName()) return this;
	
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

	GraphicsScene* scene = this->getGraphicsScene();
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

bool ot::GraphicsItem::graphicsItemRequiresHover(void) const {
	OTAssertNullptr(m_config);
	return !this->getGraphicsItemToolTip().empty();
}

// ###############################################################################################################################################

// Event handler

void ot::GraphicsItem::handleMousePressEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_parent) {
		m_parent->handleMousePressEvent(_event);
	}
	else if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable) {
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

		if ((this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsMoveable) && _event->modifiers() != Qt::ControlModifier) {
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
	else if ((this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsMoveable) && _event->modifiers() != Qt::ControlModifier) {
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
	if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHasNoFeedback) {
		return;
	}

	this->m_state |= GraphicsItem::HoverState;
	this->getQGraphicsItem()->update();
	if (m_highlightItem) {
		m_highlightItem->handleHoverEnterEvent(_event);
	}
}

void ot::GraphicsItem::handleToolTip(QGraphicsSceneHoverEvent* _event) {
	if (!this->getGraphicsItemToolTip().empty())
	{
		ToolTipHandler::showToolTip(_event->screenPos(), QString::fromStdString(this->getGraphicsItemToolTip()), 1500);
	}
	else if ((this->getGraphicsItemFlags() & GraphicsItemCfg::ItemForwardsTooltip) && m_parent) {
		m_parent->handleToolTip(_event);
	}
}

void ot::GraphicsItem::handleHoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	ToolTipHandler::hideToolTip();
	m_state &= ~(GraphicsItem::HoverState);
	this->getQGraphicsItem()->update();
	if (m_highlightItem) {
		m_highlightItem->handleHoverLeaveEvent(_event);
	}
}

void ot::GraphicsItem::paintStateBackground(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHasNoFeedback) return;

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
	QSizeF adjustedSize = _sizeHint.expandedTo(this->getGraphicsItemMinimumSize()).boundedTo(this->getGraphicsItemMaximumSize());

	switch (_hint)
	{
	case Qt::MinimumSize:
		return this->applyGraphicsItemMargins(this->getGraphicsItemMinimumSize());
	case Qt::PreferredSize:
	case Qt::MinimumDescent:
	case Qt::NSizeHints:
		break;
	case Qt::MaximumSize:
		return this->getGraphicsItemMaximumSize();
	default:
		OT_LOG_WA("Unknown Qt SizeHint");
	}
	return this->applyGraphicsItemMargins(adjustedSize.boundedTo(this->getGraphicsItemMaximumSize()));
}

QRectF ot::GraphicsItem::handleGetGraphicsItemBoundingRect(const QRectF& _rect) const {
	if (this->getGraphicsItemSizePolicy() == ot::Dynamic) {
		return QRectF(
			_rect.topLeft(),
			this->applyGraphicsItemMargins(this->getGraphicsItemMinimumSize().expandedTo(this->removeGraphicsItemMargins(m_requestedSize)).boundedTo(this->getGraphicsItemMaximumSize()))
		);
	}
	else {
		return QRectF(
			_rect.topLeft(),
			this->applyGraphicsItemMargins(_rect.size().expandedTo(this->getGraphicsItemMinimumSize()).expandedTo(this->removeGraphicsItemMargins(m_requestedSize)).boundedTo(this->getGraphicsItemMaximumSize()))
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
				m_highlightItem->setStateFlags(m_highlightItem->getStateFlags() | SelectedState);
				m_highlightItem->update();
			}
		}
		else if (!this->getQGraphicsItem()->isSelected() && (m_state & SelectedState)) {
			m_state &= (~SelectedState);
			if (m_highlightItem) {
				m_highlightItem->setStateFlags(m_highlightItem->getStateFlags() & (~SelectedState));
				m_highlightItem->update();
			}
		}
		break;
	case QGraphicsItem::ItemScenePositionHasChanged:
	{
		if ((this->getGraphicsItemFlags() & GraphicsItemCfg::ItemSnapsToGrid) && !m_parent) {
			QPointF pos = m_scene->snapToGrid(this->getQGraphicsItem()->pos());
			if (pos != this->getQGraphicsItem()->pos()) {
				this->getQGraphicsItem()->setPos(pos);
				this->getGraphicsScene()->update();
				return;
			}
		}

		for (auto c : m_connections) {
			c->updateConnection();
		}
		this->raiseEvent(ot::GraphicsItem::ItemMoved);
	}
		break;
	default:
		break;
	}
}

void ot::GraphicsItem::handleSetItemGeometry(const QRectF& _geom) {
	if (m_parent) {
		if (m_parent->getStateFlags() & GraphicsItem::ForwardSizeState) {
			this->setGraphicsItemRequestedSize(_geom.size());
		}
	}
}

void ot::GraphicsItem::raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event) {
	for (auto itm : m_eventHandler) {
		itm->graphicsItemEventHandler(this, _event);
	}
}

QRectF ot::GraphicsItem::calculatePaintArea(const QSizeF& _innerSize) {
	auto qitm = this->getQGraphicsItem();
	OTAssertNullptr(qitm);

	// Get the bounding rect of the item
	QRectF r(qitm->boundingRect());

	// Adjust size
	QSizeF inner;

	switch (this->getGraphicsItemSizePolicy())
	{
	case ot::Preferred:
		inner = _innerSize;
		break;
	case ot::Dynamic:
		inner = this->getGraphicsItemMinimumSize();
		break;
	default:
		break;
	}

	// Calculate the size of the inner rectangle
	inner = inner.expandedTo(this->getGraphicsItemMinimumSize()).expandedTo(this->removeGraphicsItemMargins(m_requestedSize)).boundedTo(this->getGraphicsItemMaximumSize());

	if (inner.toSize() == r.size()) {
		// No further adjustments needed
		return r;
	}
	else {
		// Calculate the inner rectangle
		return ot::calculateChildRect(r, inner, this->getGraphicsItemAlignment());
	}
}

// ###############################################################################################################################################

// Getter / Setter

ot::GraphicsItem* ot::GraphicsItem::getRootItem(void) {
	if (m_parent) {
		return m_parent->getRootItem();
	}
	else {
		return this;
	}
}

void ot::GraphicsItem::setGraphicsItemFlag(ot::GraphicsItemCfg::GraphicsItemFlag _flag, bool _active) {
	OTAssertNullptr(m_config);
	m_config->setGraphicsItemFlag(_flag, _active);
	this->graphicsItemFlagsChanged(this->getGraphicsItemFlags());
}

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItemCfg::GraphicsItemFlags _flags) {
	OTAssertNullptr(m_config);
	m_config->setGraphicsItemFlags(_flags);
	this->graphicsItemFlagsChanged(this->getGraphicsItemFlags());
}

const ot::GraphicsItemCfg::GraphicsItemFlags& ot::GraphicsItem::getGraphicsItemFlags(void) const {
	OTAssertNullptr(m_config);
	return m_config->getGraphicsItemFlags();
}

void ot::GraphicsItem::setGraphicsItemUid(const ot::UID& _uid) {
	OTAssertNullptr(m_config);
	m_config->setUid(_uid);
}

const ot::UID& ot::GraphicsItem::getGraphicsItemUid(void) const {
	OTAssertNullptr(m_config);
	return m_config->getUid();
}

void ot::GraphicsItem::setGraphicsItemName(const std::string& _name) {
	OTAssertNullptr(m_config);
	m_config->setName(_name);
}

const std::string& ot::GraphicsItem::getGraphicsItemName(void) const {
	OTAssertNullptr(m_config);
	return m_config->getName();
}

void ot::GraphicsItem::setGraphicsItemToolTip(const std::string& _toolTip) {
	OTAssertNullptr(m_config);
	m_config->setToolTip(_toolTip);
}

const std::string& ot::GraphicsItem::getGraphicsItemToolTip(void) const {
	OTAssertNullptr(m_config);
	return m_config->getToolTip();
}

void ot::GraphicsItem::setGraphicsItemMinimumSize(const QSizeF& _size) {
	OTAssertNullptr(m_config);
	m_config->setMinimumSize(Size2DD(_size.width(), _size.height()));
}

QSizeF ot::GraphicsItem::getGraphicsItemMinimumSize(void) const {
	OTAssertNullptr(m_config);
	return std::move(QtFactory::toSize(m_config->getMinimumSize()));
}

void ot::GraphicsItem::setGraphicsItemMaximumSize(const QSizeF& _size) {
	OTAssertNullptr(m_config);
	m_config->setMaximumSize(Size2DD(_size.width(), _size.height()));
}

QSizeF ot::GraphicsItem::getGraphicsItemMaximumSize(void) const {
	OTAssertNullptr(m_config);
	return std::move(QtFactory::toSize(m_config->getMaximumSize()));
}

void ot::GraphicsItem::setGraphicsItemSizePolicy(ot::SizePolicy _policy) {
	OTAssertNullptr(m_config);
	m_config->setSizePolicy(_policy);
}

ot::SizePolicy ot::GraphicsItem::getGraphicsItemSizePolicy(void) const {
	OTAssertNullptr(m_config);
	return m_config->getSizePolicy();
}

void ot::GraphicsItem::setGraphicsItemAlignment(ot::Alignment _align) {
	OTAssertNullptr(m_config);
	m_config->setAlignment(_align);
}

ot::Alignment ot::GraphicsItem::getGraphicsItemAlignment(void) const {
	OTAssertNullptr(m_config);
	return m_config->getAlignment();
}

void ot::GraphicsItem::setGraphicsItemMargins(const ot::MarginsD& _margins) {
	OTAssertNullptr(m_config);
	m_config->setMargins(_margins);
}

const ot::MarginsD& ot::GraphicsItem::getGraphicsItemMargins(void) const {
	OTAssertNullptr(m_config);
	return m_config->getMargins();
}

void ot::GraphicsItem::setConnectionDirection(ot::ConnectionDirection _direction) {
	OTAssertNullptr(m_config);
	m_config->setConnectionDirection(_direction);
}

ot::ConnectionDirection ot::GraphicsItem::getConnectionDirection(void) const {
	OTAssertNullptr(m_config);
	return m_config->getConnectionDirection();
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

QSizeF ot::GraphicsItem::applyGraphicsItemMargins(const QSizeF& _size) const {
	return QSizeF(
		_size.width() + this->getGraphicsItemMargins().left() + this->getGraphicsItemMargins().right(), // w
		_size.height() + this->getGraphicsItemMargins().top() + this->getGraphicsItemMargins().bottom() // h
	);
}

QSizeF ot::GraphicsItem::removeGraphicsItemMargins(const QSizeF& _size) const {
	return QSizeF(
		(_size.width() - this->getGraphicsItemMargins().left()) - this->getGraphicsItemMargins().right(), // w
		(_size.height() - this->getGraphicsItemMargins().top()) - this->getGraphicsItemMargins().bottom() // h
	);
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
