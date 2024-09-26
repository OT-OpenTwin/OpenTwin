//! @file GraphicsItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Math.h"
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/Positioning.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/ToolTipHandler.h"
#include "OTWidgets/GraphicsStackItem.h"
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

// ###############################################################################################################################################

// Public Static

ot::Painter2D* ot::GraphicsItem::createSelectionBorderPainter(void) {
	return new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemSelectionBorder);
}

ot::Painter2D* ot::GraphicsItem::createHoverBorderPainter(void) {
	return new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemHoverBorder);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor / Destructor

ot::GraphicsItem::GraphicsItem(GraphicsItemCfg* _configuration)
	: m_config(_configuration), m_moveStartPt(0., 0.), m_parent(nullptr), m_requestedSize(-1., -1.),
	m_blockConfigurationNotifications(false), m_blockFlagNotifications(false), m_forwardSizeChanges(false)
{

}

ot::GraphicsItem::~GraphicsItem() {
	if (this->getGraphicsScene()) {
		this->getGraphicsScene()->itemAboutToBeRemoved(this);
	}

	for (GraphicsConnectionItem* connection : m_connections) {
		connection->forgetItem(this);
	}
}

// ###############################################################################################################################################

// Virtual functions

bool ot::GraphicsItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	OTAssertNullptr(this->getQGraphicsItem());
	
	m_blockConfigurationNotifications = true;
	this->setConfiguration(_cfg->createCopy());

	OTAssertNullptr(m_config);

	if (m_config) {
		m_moveStartPt = QPointF(m_config->getPosition().x(), m_config->getPosition().y());
		this->setGraphicsItemName(this->getGraphicsItemName());
		if (!m_blockFlagNotifications) this->graphicsItemFlagsChanged(this->getGraphicsItemFlags());
	}
	m_blockConfigurationNotifications = false;

	return true;
}

void ot::GraphicsItem::graphicsItemFlagsChanged(const GraphicsItemCfg::GraphicsItemFlags& _flags) {
	this->getQGraphicsItem()->setFlag(QGraphicsItem::ItemIsMovable, _flags & GraphicsItemCfg::ItemIsMoveable);
	this->getQGraphicsItem()->setFlag(QGraphicsItem::ItemIsSelectable, _flags & GraphicsItemCfg::ItemIsSelectable);
}

void ot::GraphicsItem::graphicsItemConfigurationChanged(const GraphicsItemCfg* _config) {
	if (m_blockConfigurationNotifications) return;
	if (!this->getGraphicsScene()) return;
	OTAssertNullptr(this->getGraphicsScene()->getGraphicsView());
	this->getGraphicsScene()->getGraphicsView()->notifyItemConfigurationChanged(this);
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
		graphicConnectionCfgList.push_back(connection->getConfiguration());
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
		return;
	}

	if ((this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsMoveable) && _event->modifiers() != Qt::ControlModifier) {
		this->setBlockConfigurationNotifications(true);
		auto qitm = this->getQGraphicsItem();
		OTAssertNullptr(qitm);
		m_moveStartPt = qitm->pos(); // The item is root item, so pos returns the scene pos
	}
	
	if ((this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsSelectable) && !this->getQGraphicsItem()->isSelected()) {
		GraphicsScene* sc = this->getGraphicsScene();
		if (!sc) {
			OT_LOG_EA("No scene set");
			return;
		}

		bool ignored = sc->getIgnoreEvents();
		bool blocked = sc->signalsBlocked();
		sc->setIgnoreEvents(true);
		sc->blockSignals(true);

		if (_event->modifiers() != Qt::ControlModifier || !sc->getMultiselectionEnabled()) {
			sc->deselectAll();
		}
		this->getQGraphicsItem()->setSelected(true);

		sc->setIgnoreEvents(ignored);
		sc->blockSignals(blocked);

		this->handleItemChange(QGraphicsItem::ItemSelectedHasChanged, QVariant());

		sc->handleSelectionChanged();
	}
}

void ot::GraphicsItem::handleMouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_parent) {
		m_parent->handleMouseReleaseEvent(_event);
		return;
	}

	if ((this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsMoveable)) {
		this->setBlockConfigurationNotifications(false);
		this->notifyMoveIfRequired();
	}
}

void ot::GraphicsItem::handleHoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleToolTip(_event);
	//this->setGraphicsElementState(GraphicsElement::HoverState, true);
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

	//this->setGraphicsElementState(GraphicsElement::HoverState, false);
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
		if (this->getQGraphicsItem()->isSelected()) {
			this->setGraphicsElementState(GraphicsElementState::SelectedState, true);
		}
		else {
			this->setGraphicsElementState(GraphicsElementState::SelectedState, false);
		}
		break;

	case QGraphicsItem::ItemScenePositionHasChanged:
	{
		if ((this->getGraphicsItemFlags() & GraphicsItemCfg::ItemSnapsToGrid) && !m_parent) {
			QPointF pos = this->getGraphicsScene()->snapToGrid(this->getQGraphicsItem()->pos());
			if (pos != this->getQGraphicsItem()->pos()) {
				this->setGraphicsItemPos(pos);
				//this->getGraphicsScene()->update();
				return;
			}
		}

		m_config->setPosition(QtFactory::toPoint2D(this->getQGraphicsItem()->pos()));

		for (auto c : m_connections) {
			c->updateConnectionView();
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
		if (m_parent->getForwardSizeChanges()) {
			this->setGraphicsItemRequestedSize(_geom.size());
		}
	}
}

void ot::GraphicsItem::raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event) {
	for (auto itm : m_eventHandler) {
		itm->graphicsItemEventHandler(this, _event);
	}
}

qreal ot::GraphicsItem::calculateShortestDistanceToPoint(const QPointF& _pt) const {
	QRectF virtualRect = this->getTriggerBoundingRect();
	if (virtualRect.contains(_pt)) return Math::euclideanDistance(virtualRect.center().x(), virtualRect.center().y(), _pt.x(), _pt.y());
	else return -1.;
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

ot::GraphicsScene* ot::GraphicsItem::getGraphicsScene(void) const {
	return (m_parent ? m_parent->getGraphicsScene() : this->GraphicsElement::getGraphicsScene());
}

ot::GraphicsItem* ot::GraphicsItem::getRootItem(void) {
	if (m_parent) {
		return m_parent->getRootItem();
	}
	else {
		return this;
	}
}

const ot::GraphicsItem* ot::GraphicsItem::getRootItem(void) const {
	if (m_parent) {
		return m_parent->getRootItem();
	}
	else {
		return this;
	}
}

void ot::GraphicsItem::setConfiguration(GraphicsItemCfg* _config) {
	OTAssertNullptr(_config);
	if (m_config == _config) return;
	if (m_config) delete m_config;
	m_config = _config;

	this->getQGraphicsItem()->setPos(QtFactory::toQPoint(m_config->getPosition()));
	this->applyGraphicsItemTransform();

	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

void ot::GraphicsItem::setGraphicsItemPos(const QPointF& _pos) {
	this->setGraphicsItemPos(QtFactory::toPoint2D(_pos));
}

void ot::GraphicsItem::setGraphicsItemPos(const Point2DD& _pos) {
	OTAssertNullptr(m_config);
	OTAssertNullptr(this->getQGraphicsItem());
	this->prepareGraphicsItemGeometryChange();
	m_config->setPosition(_pos);
	this->getQGraphicsItem()->setPos(QtFactory::toQPoint(_pos));
	this->getQGraphicsLayoutItem()->updateGeometry();

	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const ot::Point2DD& ot::GraphicsItem::getGraphicsItemPos(void) const {
	OTAssertNullptr(m_config);
	return m_config->getPosition();
}

void ot::GraphicsItem::setGraphicsItemFlag(ot::GraphicsItemCfg::GraphicsItemFlag _flag, bool _active) {
	OTAssertNullptr(m_config);
	m_config->setGraphicsItemFlag(_flag, _active);
	if (!m_blockFlagNotifications) this->graphicsItemFlagsChanged(this->getGraphicsItemFlags());
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItemCfg::GraphicsItemFlags _flags) {
	OTAssertNullptr(m_config);
	m_config->setGraphicsItemFlags(_flags);
	if (!m_blockFlagNotifications) this->graphicsItemFlagsChanged(this->getGraphicsItemFlags());
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const ot::GraphicsItemCfg::GraphicsItemFlags& ot::GraphicsItem::getGraphicsItemFlags(void) const {
	OTAssertNullptr(m_config);
	return m_config->getGraphicsItemFlags();
}

void ot::GraphicsItem::setGraphicsItemUid(const ot::UID& _uid) {
	OTAssertNullptr(m_config);
	m_config->setUid(_uid);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const ot::UID& ot::GraphicsItem::getGraphicsItemUid(void) const {
	OTAssertNullptr(m_config);
	return m_config->getUid();
}

void ot::GraphicsItem::setGraphicsItemName(const std::string& _name) {
	OTAssertNullptr(m_config);
	m_config->setName(_name);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const std::string& ot::GraphicsItem::getGraphicsItemName(void) const {
	OTAssertNullptr(m_config);
	return m_config->getName();
}

void ot::GraphicsItem::setGraphicsItemToolTip(const std::string& _toolTip) {
	OTAssertNullptr(m_config);
	m_config->setToolTip(_toolTip);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const std::string& ot::GraphicsItem::getGraphicsItemToolTip(void) const {
	OTAssertNullptr(m_config);
	return m_config->getToolTip();
}

void ot::GraphicsItem::setAdditionalTriggerDistance(const ot::MarginsD& _distance) {
	OTAssertNullptr(m_config);
	m_config->setAdditionalTriggerDistance(_distance);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const ot::MarginsD& ot::GraphicsItem::getAdditionalTriggerDistance(void) const {
	OTAssertNullptr(m_config);
	return m_config->getAdditionalTriggerDistance();
}

double ot::GraphicsItem::getMaxAdditionalTriggerDistance(void) const {
	const ot::MarginsD& td = this->getAdditionalTriggerDistance();
	return std::max({ td.left(), td.top(), td.right(), td.bottom() });
}

void ot::GraphicsItem::setGraphicsItemMinimumSize(const QSizeF& _size) {
	OTAssertNullptr(m_config);
	m_config->setMinimumSize(Size2DD(_size.width(), _size.height()));
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

QSizeF ot::GraphicsItem::getGraphicsItemMinimumSize(void) const {
	OTAssertNullptr(m_config);
	return std::move(QtFactory::toQSize(m_config->getMinimumSize()));
}

void ot::GraphicsItem::setGraphicsItemMaximumSize(const QSizeF& _size) {
	OTAssertNullptr(m_config);
	m_config->setMaximumSize(Size2DD(_size.width(), _size.height()));
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

QSizeF ot::GraphicsItem::getGraphicsItemMaximumSize(void) const {
	OTAssertNullptr(m_config);
	return std::move(QtFactory::toQSize(m_config->getMaximumSize()));
}

void ot::GraphicsItem::setGraphicsItemSizePolicy(ot::SizePolicy _policy) {
	OTAssertNullptr(m_config);
	m_config->setSizePolicy(_policy);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

ot::SizePolicy ot::GraphicsItem::getGraphicsItemSizePolicy(void) const {
	OTAssertNullptr(m_config);
	return m_config->getSizePolicy();
}

void ot::GraphicsItem::setGraphicsItemAlignment(ot::Alignment _align) {
	OTAssertNullptr(m_config);
	m_config->setAlignment(_align);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

ot::Alignment ot::GraphicsItem::getGraphicsItemAlignment(void) const {
	OTAssertNullptr(m_config);
	return m_config->getAlignment();
}

void ot::GraphicsItem::setGraphicsItemMargins(const ot::MarginsD& _margins) {
	OTAssertNullptr(m_config);
	m_config->setMargins(_margins);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const ot::MarginsD& ot::GraphicsItem::getGraphicsItemMargins(void) const {
	OTAssertNullptr(m_config);
	return m_config->getMargins();
}

void ot::GraphicsItem::setConnectionDirection(ot::ConnectionDirection _direction) {
	OTAssertNullptr(m_config);
	m_config->setConnectionDirection(_direction);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

ot::ConnectionDirection ot::GraphicsItem::getConnectionDirection(void) const {
	OTAssertNullptr(m_config);
	switch (m_config->getConnectionDirection()) {
	case ot::ConnectAny: return this->calculateOutwardsConnectionDirection();
	case ot::ConnectLeft: return ConnectionDirection::ConnectLeft;
	case ot::ConnectUp: return ConnectionDirection::ConnectUp;
	case ot::ConnectRight: return ConnectionDirection::ConnectRight;
	case ot::ConnectDown: return ConnectionDirection::ConnectDown;
	case ot::ConnectOut: return this->calculateOutwardsConnectionDirection();
	case ot::ConnectIn: return ot::inversedConnectionDirection(this->calculateOutwardsConnectionDirection());
	default:
		OT_LOG_EA("Unknown connection direction");
		return this->calculateOutwardsConnectionDirection();
	}
}

void ot::GraphicsItem::setStringMap(const std::map<std::string, std::string>& _map) {
	OTAssertNullptr(m_config);
	m_config->setStringMap(_map);
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const std::map<std::string, std::string>& ot::GraphicsItem::getStringMap(void) const {
	OTAssertNullptr(m_config);
	return m_config->getStringMap();
}

void ot::GraphicsItem::setGraphicsItemTransform(const Transform& _transform) {
	OTAssertNullptr(m_config);
	m_config->setTransform(_transform);
	this->applyGraphicsItemTransform();
	this->getQGraphicsItem()->update();
	for (GraphicsConnectionItem* con : m_connections) {
		con->update();
	}
	if (!m_blockConfigurationNotifications) this->graphicsItemConfigurationChanged(m_config);
}

const ot::Transform& ot::GraphicsItem::getGraphicsItemTransform(void) const {
	OTAssertNullptr(m_config);
	return m_config->getTransform();
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
		graphicConnectionCfgList.push_back(connection->getConfiguration());
	}

	for (auto childQGraphicsItem : getQGraphicsItem()->childItems()) 
	{
		ot::GraphicsItem* graphicsItem = dynamic_cast<ot::GraphicsItem*>(childQGraphicsItem);
		if (graphicsItem != nullptr)
		{
			graphicConnectionCfgList.splice(graphicConnectionCfgList.end(), graphicsItem->getConnectionCfgs());
		}
	}
	return graphicConnectionCfgList;
}

void ot::GraphicsItem::setGraphicsItemSelected(bool _selected) {
	this->setGraphicsElementState(GraphicsElement::SelectedState, _selected);
	this->getQGraphicsItem()->setSelected(_selected);
}

bool ot::GraphicsItem::getGraphicsItemSelected(void) const {
	return this->getGraphicsElementState() & GraphicsElement::SelectedState;
}

void ot::GraphicsItem::setCurrentPosAsMoveStart(void) {
	OTAssertNullptr(this->getQGraphicsItem());
	m_moveStartPt = this->getQGraphicsItem()->pos();
}

void ot::GraphicsItem::notifyMoveIfRequired(void) {
	if (m_blockConfigurationNotifications) return;
	OTAssertNullptr(this->getQGraphicsItem());
	OTAssertNullptr(this->getGraphicsScene());
	OTAssertNullptr(this->getGraphicsScene()->getGraphicsView());
	if (m_moveStartPt != this->getQGraphicsItem()->pos()) {
		this->getGraphicsScene()->getGraphicsView()->notifyItemMoved(this);
		this->getGraphicsScene()->getGraphicsView()->notifyItemConfigurationChanged(this);
	}
}

void ot::GraphicsItem::parentItemTransformChanged(const QTransform& _parentTransform) {
	if (!(this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIgnoresParentTransform)) return;
	return;

	// Calculate inverse transform of parent
	QPointF transformOrigin;
	QTransform ownTransform = this->calculateGraphicsItemTransform(transformOrigin);

	QTransform invertedParentTransform = _parentTransform.inverted();
	
	// Combine with the item's own transformation
	QTransform newTransform = invertedParentTransform * ownTransform;

	QTransform centeredTransform;
	centeredTransform.translate(transformOrigin.x(), transformOrigin.y());
	centeredTransform *= newTransform;
	centeredTransform.translate(-transformOrigin.x(), -transformOrigin.y());

	//this->getQGraphicsItem()->setTransformOriginPoint(transformOrigin);
	this->getQGraphicsItem()->setTransform(centeredTransform);
}

QRectF ot::GraphicsItem::getTriggerBoundingRect(void) const {
	OTAssertNullptr(this->getQGraphicsItem());
	OTAssertNullptr(this->getConfiguration());
	QRectF rec = this->getQGraphicsItem()->boundingRect();
	rec.moveTo(this->getQGraphicsItem()->scenePos());
	return rec.marginsAdded(QtFactory::toQMargins(this->getConfiguration()->getAdditionalTriggerDistance()));
}

void ot::GraphicsItem::graphicsElementStateChanged(const GraphicsElementStateFlags& _state) {
	GraphicsElement::graphicsElementStateChanged(_state);
	this->getQGraphicsItem()->update();
}

void ot::GraphicsItem::applyGraphicsItemTransform(void) {
	OTAssertNullptr(this->getQGraphicsItem());
	
	// Apply transform
	QPointF transformOrigin;
	QTransform newTransform = this->calculateGraphicsItemTransform(transformOrigin);
	this->getQGraphicsItem()->setTransformOriginPoint(transformOrigin);
	this->getQGraphicsItem()->setTransform(newTransform);

	this->notifyChildsAboutTransformChange(newTransform);
}

QTransform ot::GraphicsItem::calculateGraphicsItemTransform(QPointF& _transformOrigin) const {
	QTransform newTransform;
	_transformOrigin = this->getQGraphicsItem()->boundingRect().center();

	// Adjust transformation origin
	newTransform.translate(_transformOrigin.x(), _transformOrigin.y());

	// Flip
	if (this->getGraphicsItemTransform().getFlipStateFlags() & Transform::FlipHorizontally) {
		newTransform.scale(-1, 1);
	}
	if (this->getGraphicsItemTransform().getFlipStateFlags() & Transform::FlipVertically) {
		newTransform.scale(1, -1);
	}

	// Rotate
	newTransform.rotate(this->getGraphicsItemTransform().getRotation());

	// Translate back
	newTransform.translate(-_transformOrigin.x(), -_transformOrigin.y());

	return newTransform;
}

ot::ConnectionDirection ot::GraphicsItem::calculateOutwardsConnectionDirection(void) const {
	OTAssertNullptr(this->getQGraphicsItem());
	QPointF thisCenter = this->getQGraphicsItem()->mapToScene(this->getQGraphicsItem()->boundingRect().center());
	QPointF rootCenter = this->getRootItem()->getQGraphicsItem()->mapToScene(this->getRootItem()->getQGraphicsItem()->boundingRect().center());
	qreal dx = thisCenter.x() - rootCenter.x();
	qreal dy = thisCenter.y() - rootCenter.y();
	
	if (dx == 0. && dy == 0.) {
		return ConnectionDirection::ConnectAny;
	}

	bool isRight = true;
	if (dx < 0.) {
		dx = dx * (-1.);
		isRight = false;
	}

	bool isDown = true;
	if (dy < 0.) {
		dy = dy * (-1.);
		isDown = false;
	}
	
	if (dx >= dy) {
		if (isRight) return ConnectionDirection::ConnectRight;
		else return ConnectionDirection::ConnectLeft;
	}
	else {
		if (isDown) return ConnectionDirection::ConnectDown;
		else return ConnectionDirection::ConnectUp;
	}
}
