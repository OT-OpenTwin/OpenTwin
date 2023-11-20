//! @file GraphicsItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsStackItem.h"
#include "OTWidgets/GraphicsItemDrag.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/KeyMap.h"
#include "OTCore/Logger.h"

#include <QtCore/qmimedata.h>
#include <QtCore/qmath.h>
#include <QtGui/qdrag.h>
#include <QtGui/qfont.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qstyleoption.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qwidget.h>

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsItem::GraphicsItem(bool _isLayoutOrStack)
	: m_flags(GraphicsItem::NoFlags), m_drag(nullptr), m_parent(nullptr), m_isLayoutOrStack(_isLayoutOrStack), 
	m_hasHover(false), m_scene(nullptr), m_alignment(ot::AlignCenter), m_minSize(0., 0.), m_maxSize(DBL_MAX, DBL_MAX),
	m_sizePolicy(ot::Preferred), m_requestedSize(-1., -1.), m_connectionDirection(ot::ConnectAny)
{

}

ot::GraphicsItem::~GraphicsItem() {
	if (m_drag) delete m_drag;
}

// ###############################################################################################################################################

// Virtual functions

bool ot::GraphicsItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	if (_cfg->graphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable) { m_flags |= GraphicsItem::ItemIsConnectable; }
	m_uid = _cfg->uid();
	this->setGraphicsItemName(_cfg->name());	
	m_alignment = _cfg->alignment();
	m_sizePolicy = _cfg->sizePolicy();
	m_margins = _cfg->margins();
	m_minSize = QSizeF(_cfg->minimumSize().width(), _cfg->minimumSize().height());
	m_maxSize = QSizeF(_cfg->maximumSize().width(), _cfg->maximumSize().height());
	m_moveStartPt = QPointF(_cfg->position().x(), _cfg->position().y());
	m_connectionDirection = _cfg->connectionDirection();
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

	std::list<ot::GraphicsConnectionCfg> lst;
	for (const auto& connection : m_connections) {
		lst.push_back(connection->getConnectionInformation());
	}
	for (const auto& connection : lst) {	
		view->removeConnection(connection);
	}
}

// ###############################################################################################################################################

// Event handler

void ot::GraphicsItem::handleMousePressEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_parent) {
		m_parent->handleMousePressEvent(_event);
	}
	else if (m_flags & ot::GraphicsItem::ItemPreviewContext) {
		if (_event->button() == Qt::LeftButton) {
			if (m_drag == nullptr) {
				m_drag = new GraphicsItemDrag(this);
			}
			m_drag->queue(_event->widget());
		}
	}
	else if (m_flags & ot::GraphicsItem::ItemNetworkContext) {
		if (m_flags & ot::GraphicsItem::ItemIsConnectable) {
			OTAssertNullptr(m_scene); // Ensure the finalizeItem() method calls setGraphicsScene()
			m_scene->startConnection(this);
		}
		else {
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
	else if (m_flags & ot::GraphicsItem::ItemNetworkContext) {
		if (m_flags & ot::GraphicsItem::ItemIsConnectable) {
		}
		else {
			auto qitm = this->getQGraphicsItem();
			OTAssertNullptr(qitm);
			// Check if the item has moved after the user released the mouse
			if (qitm->pos() != m_moveStartPt) {
				OTAssertNullptr(m_scene);
				m_scene->getGraphicsView()->notifyItemMoved(this);
			}
		}
	}
}

void ot::GraphicsItem::paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (m_hasHover && (m_flags & GraphicsItem::ItemIsConnectable)) {
		_painter->fillRect(this->getQGraphicsItem()->boundingRect(), Qt::GlobalColor::green);
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
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		for (auto c : m_connections) {
			c->updateConnection();
		}
		this->raiseEvent(ot::GraphicsItem::ItemMoved);
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

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags) {
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
	auto it = std::find(m_connections.begin(), m_connections.end(), _connection);
	while (it != m_connections.end()) {
		m_connections.erase(it);
		it = std::find(m_connections.begin(), m_connections.end(), _connection);
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

	inner = inner.expandedTo(m_minSize).expandedTo(this->removeGraphicsItemMargins(m_requestedSize)).boundedTo(m_maxSize);

	// No further adjustments needed
	if (inner.toSize() == r.size()) return r;

	QPointF pt(r.topLeft());

	// Align inner size
	switch (m_alignment)
	{
	case ot::AlignCenter:
		pt.setX(pt.x() + ((r.size().width() - inner.width()) / 2.));
		pt.setY(pt.y() + ((r.size().height() - inner.height()) / 2.));
		break;
	case ot::AlignTop:
		pt.setX(pt.x() + ((r.size().width() - inner.width()) / 2.));
		break;
	case ot::AlignTopRight:
		pt.setX(pt.x() + (r.size().width() - inner.width()));
		break;
	case ot::AlignRight:
		pt.setX(pt.x() + (r.size().width() - inner.width()));
		pt.setY(pt.y() + ((r.size().height() - inner.height()) / 2.));
		break;
	case ot::AlignBottomRight:
		pt.setX(pt.x() + (r.size().width() - inner.width()));
		pt.setY(pt.y() + (r.size().height() - inner.height()));
		break;
	case ot::AlignBottom:
		pt.setX(pt.x() + ((r.size().width() - inner.width()) / 2.));
		pt.setY(pt.y() + (r.size().height() - inner.height()));
		break;
	case ot::AlignBottomLeft:
		pt.setY(pt.y() + (r.size().height() - inner.height()));
		break;
	case ot::AlignLeft:
		pt.setY(pt.y() + ((r.size().height() - inner.height()) / 2.));
		break;
	case ot::AlignTopLeft:
		break;
	default:
		OT_LOG_EA("Unknown Alignment");
		break;
	}

	return QRectF(pt, inner);
}
