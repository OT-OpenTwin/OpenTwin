//! @file GraphicsItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/OTQtConverter.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinCore/KeyMap.h"

#include <QtCore/qmimedata.h>
#include <QtCore/qmath.h>
#include <QtGui/qdrag.h>
#include <QtGui/qfont.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qstyleoption.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qwidget.h>

ot::GraphicsItemDrag::GraphicsItemDrag(GraphicsItem* _owner) : m_widget(nullptr), m_owner(_owner), m_queueCount(0) {}

ot::GraphicsItemDrag::~GraphicsItemDrag() {}

void ot::GraphicsItemDrag::queue(QWidget* _widget) {
	m_queueCount++;
	m_widget = _widget;
	QMetaObject::invokeMethod(this, &GraphicsItemDrag::slotQueue, Qt::QueuedConnection);
}

void ot::GraphicsItemDrag::slotQueue(void) {
	if (--m_queueCount == 0) {
		// Add configuration to mime data
		QDrag drag(m_widget);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText("OT_BLOCK");
		mimeData->setData(OT_GRAPHICSITEM_MIMETYPE_ItemName, QByteArray::fromStdString(m_owner->graphicsItemName()));

		// Create drag
		drag.setMimeData(mimeData);

		// Create preview
		QPixmap prev(m_owner->getQGraphicsItem()->boundingRect().size().toSize());
		QPainter p(&prev);
		QStyleOptionGraphicsItem opt;
		p.fillRect(QRect(QPoint(0, 0), m_owner->getQGraphicsItem()->boundingRect().size().toSize()), Qt::gray);

		// Paint
		m_owner->callPaint(&p, &opt, m_widget);

		// Run drag
		drag.setPixmap(prev);
		drag.exec();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsItem::GraphicsItem() : m_flags(GraphicsItem::NoFlags), m_drag(nullptr), m_parent(nullptr), 
	m_hasHover(false), m_scene(nullptr), m_alignment(ot::AlignCenter), m_minSize(0., 0.), m_maxSize(DBL_MAX, DBL_MAX)
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
	m_name = _cfg->name();
	m_alignment = _cfg->alignment();
	m_margins = _cfg->margins();
	m_minSize = QSizeF(_cfg->minimumSize().width(), _cfg->minimumSize().height());
	m_maxSize = QSizeF(_cfg->maximumSize().width(), _cfg->maximumSize().height());
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

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags) {
	m_flags = _flags;
	this->graphicsItemFlagsChanged(m_flags);
}

ot::GraphicsScene* ot::GraphicsItem::graphicsScene(void) {
	if (m_parent) return m_parent->graphicsScene();
	return m_scene;
}

ot::GraphicsItem* ot::GraphicsItem::getRootItem(void) {
	if (m_parent) return m_parent->getRootItem();
	return this;
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

void ot::GraphicsItem::handleItemMoved(void) {
	for (auto c : m_connections) c->updateConnection();
	this->raiseEvent(ot::GraphicsItem::ItemMoved);

	// For root items we notify the view
	if (m_parent == nullptr) {
		otAssert(!m_uid.empty(), "Root items should always have a valid uid");
		OTAssertNullptr(m_scene); // Scene was not set when adding this item (all root items should have their scene set)
		m_scene->getGraphicsView()->notifyItemMoved(this);
	}
}

void ot::GraphicsItem::paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (m_hasHover && (m_flags & GraphicsItem::ItemIsConnectable)) {
		_painter->fillRect(this->getQGraphicsItem()->boundingRect(), Qt::GlobalColor::green);
	}
}

QSizeF ot::GraphicsItem::handleGetGraphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _sizeHint) const {
	// The adjusted size is the size hint expanded to the minimum size, bound to maximum size and with margins applied
	QSizeF adjustedSize = this->applyGraphicsItemMargins(_sizeHint).expandedTo(m_minSize).boundedTo(m_maxSize);

	switch (_hint)
	{
	case Qt::MinimumSize: 
	case Qt::PreferredSize:
	case Qt::MinimumDescent:
	case Qt::NSizeHints:
		return adjustedSize;
	case Qt::MaximumSize:
		return adjustedSize.expandedTo(m_requestedSize).boundedTo(m_maxSize); // Stretch to requested size
	default:
		OT_LOG_WA("Unknown Qt SizeHint");
		return adjustedSize;
	}
}

QRectF ot::GraphicsItem::handleGetGraphicsItemBoundingRect(const QRectF& _rect) const {
	return QRectF(
		_rect.topLeft(), 
		this->applyGraphicsItemMargins(_rect.size()).expandedTo(m_minSize).expandedTo(m_requestedSize).boundedTo(m_maxSize)
	);
}

// ###############################################################################################################################################

// Getter / Setter

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
	this->prepareGraphicsItemGeometryChange();
	m_requestedSize = _size;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Helper

/*
QRectF ot::GraphicsItem::calculateInnerRect(const QSizeF& _innerMinSize, const QSizeF& _innerMaxSize) const {
	// Calculate size
	QSizeF minSize = _innerMinSize + QSizeF(m_margins.left() + m_margins.right(), m_margins.top() + m_margins.bottom());
	QSizeF maxSize = _innerMaxSize + QSizeF(m_margins.left() + m_margins.right(), m_margins.top() + m_margins.bottom());
	QSizeF s = minSize.expandedTo(m_geometryRect.size()).boundedTo(maxSize);

	// Position
	QPointF p; // Top left coord
	switch (m_alignment)
	{
	case ot::AlignCenter:
		p.setX(m_geometryRect.x() + ((m_geometryRect.width() / 2.) - (s.width() / 2.)));
		p.setY(m_geometryRect.y() + ((m_geometryRect.height() / 2.) - (s.height() / 2.)));
		break;
	case ot::AlignTop:
		p.setX(m_geometryRect.x() + ((m_geometryRect.width() / 2.) - (s.width() / 2.)));
		p.setY(m_geometryRect.y());
		break;
	case ot::AlignTopRight:
		p.setX(m_geometryRect.x() + (m_geometryRect.width() - s.width()));
		p.setY(m_geometryRect.y());
		break;
	case ot::AlignRight:
		p.setX(m_geometryRect.x() + (m_geometryRect.width() - s.width()));
		p.setY(m_geometryRect.y() + ((m_geometryRect.height() / 2.) - (s.height() / 2.)));
		break;
	case ot::AlignBottomRight:
		p.setX(m_geometryRect.x() + (m_geometryRect.width() - s.width()));
		p.setY(m_geometryRect.y() + (m_geometryRect.height() - s.height()));
		break;
	case ot::AlignBottom:
		p.setX(m_geometryRect.x() + ((m_geometryRect.width() / 2.) - (s.width() / 2.)));
		p.setY(m_geometryRect.y() + (m_geometryRect.height() - s.height()));
		break;
	case ot::AlignBottomLeft:
		p.setX(m_geometryRect.x());
		p.setY(m_geometryRect.y() + (m_geometryRect.height() - s.height()));
		break;
	case ot::AlignLeft:
		p.setX(m_geometryRect.x());
		p.setY(m_geometryRect.y() + ((m_geometryRect.height() / 2.) - (s.height() / 2.)));
	case ot::AlignTopLeft:
		p = m_geometryRect.topLeft();
		break;
	default:
		OT_LOG_EA("Unknown alignment provided");
		break;
	}

	return QRectF(p, s);
}
*/

void ot::GraphicsItem::raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event) {
	for (auto itm : m_eventHandler) {
		itm->graphicsItemEventHandler(this, _event);
	}
}

QSizeF ot::GraphicsItem::applyGraphicsItemMargins(const QSizeF& _size) const {
	return QSizeF(_size.width() + m_margins.left() + m_margins.right(), _size.height() + m_margins.top() + m_margins.bottom());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsGroupItem::GraphicsGroupItem() {
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsGroupItem::~GraphicsGroupItem() {

}

bool ot::GraphicsGroupItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsGroupItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsGroupItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->handleGetGraphicsItemSizeHint(_hint, this->boundingRect().size());
	//return this->boundingRect().size();
}

QRectF ot::GraphicsGroupItem::boundingRect(void) const {
	//return QGraphicsItemGroup::boundingRect();
	return this->handleGetGraphicsItemBoundingRect(QGraphicsItemGroup::boundingRect());
}

void ot::GraphicsGroupItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QVariant ot::GraphicsGroupItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return QGraphicsItemGroup::itemChange(_change, _value);
}

void ot::GraphicsGroupItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItemGroup::mousePressEvent(_event);
}

void ot::GraphicsGroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsGroupItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {

	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsGroupItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsItemGroup::paint(_painter, _opt, _widget);
}

void ot::GraphicsGroupItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

ot::GraphicsItem* ot::GraphicsGroupItem::findItem(const std::string& _itemName) {
	if (_itemName == this->graphicsItemName()) return this;

	for (auto i : this->childItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
		if (itm) {
			auto r = itm->findItem(_itemName);
			if (r) return r;
		}
	}
	return nullptr;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsStackItem::GraphicsStackItem() {

}

ot::GraphicsStackItem::~GraphicsStackItem() {
	this->memClear();
}

bool ot::GraphicsStackItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsStackItemCfg* cfg = dynamic_cast<ot::GraphicsStackItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->memClear();

	for (auto itm : cfg->items()) {
		OTAssertNullptr(itm.item);

		ot::GraphicsItem* i = nullptr;
		try {
			i = ot::GraphicsFactory::itemFromConfig(itm.item);
			if (i) {
				GraphicsStackItemEntry e;
				e.isMaster = itm.isMaster;
				e.item = i;
				i->setParentGraphicsItem(this);
				if (e.isMaster) {
					// If the item is a master item, install an event filter for resizing the child items
					i->addGraphicsItemEventHandler(this);
				}
				m_items.push_back(e);

				this->addToGroup(e.item->getQGraphicsItem());
			}
			else {
				OT_LOG_EA("Failed to created graphics item from factory");
			}
		}
		catch (const std::exception& _e) {
			OT_LOG_EAS("Failed to create child item: " + std::string(_e.what()));
			if (i) delete i;
			throw _e;
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
			if (i) delete i;
			throw std::exception("[FATAL] Unknown error");
		}
	}

	return ot::GraphicsGroupItem::setupFromConfig(_cfg);
}

void ot::GraphicsStackItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	ot::GraphicsGroupItem::callPaint(_painter, _opt, _widget);
	for (auto itm : m_items) itm.item->callPaint(_painter, _opt, _widget);
}

void ot::GraphicsStackItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	ot::GraphicsGroupItem::graphicsItemFlagsChanged(_flags);
}

void ot::GraphicsStackItem::graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) {
	OTAssertNullptr(_sender);
	return;
	if (_event == ot::GraphicsItem::ItemResized) {
		ot::GraphicsItem* mas = nullptr;
		for (auto itm : m_items) {
			if (itm.isMaster) {
				if (mas != nullptr) {
					OT_LOG_EA("Multiple master items not supported yet");
				}
				mas = itm.item;
			}
		}
		if (mas == nullptr) {
			OT_LOG_EA("No master item found");
		}
		else {
			for (auto itm : m_items) {
				if (itm.item != _sender && !itm.isMaster) {
					//itm.item->setGraphicsItemRequestedSize(mas->getQGraphicsItem()->boundingRect().size());
				}
			}
		}
	}
}

void ot::GraphicsStackItem::memClear(void) {
	for (auto itm : m_items) delete itm.item;
	m_items.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsRectangularItem::GraphicsRectangularItem() : m_size(10, 10), m_cornerRadius(0) {
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsRectangularItem::~GraphicsRectangularItem() {

}

bool ot::GraphicsRectangularItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsRectangularItemCfg* cfg = dynamic_cast<ot::GraphicsRectangularItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());
	m_cornerRadius = cfg->cornerRadius();
	m_brush = ot::Painter2DFactory::brushFromPainter2D(cfg->backgroundPainter());
	m_pen.setWidth(cfg->border().top()); // ToDo: Add seperate borders on all 4 sides
	m_pen.setBrush(QBrush(ot::OTQtConverter::toQt(cfg->border().color())));
	m_pen.setColor(ot::OTQtConverter::toQt(cfg->border().color()));
	
	// We call set rectangle size which will call set geometry to finalize the item
	this->setRectangleSize(m_size);

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsRectangularItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsRectangularItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		return this->handleGetGraphicsItemSizeHint(_hint, m_size);
		//return m_size;
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	return this->handleGetGraphicsItemSizeHint(_hint, m_size);
	//return _constrains;
};

QRectF ot::GraphicsRectangularItem::boundingRect(void) const {
	//return QRectF(QPointF(0., 0.), this->geometry().size());
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), m_size));
}

void ot::GraphicsRectangularItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QVariant ot::GraphicsRectangularItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return _value;
}

void ot::GraphicsRectangularItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

void ot::GraphicsRectangularItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsRectangularItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);
	_painter->drawRoundedRect(this->boundingRect(), m_cornerRadius, m_cornerRadius);
}

void ot::GraphicsRectangularItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsRectangularItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsRectangularItem::setRectangleSize(const QSizeF& _size) {
	m_size = _size;
	this->setGeometry(QRectF(this->pos(), m_size));
};

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsEllipseItem::GraphicsEllipseItem() : m_radiusX(5), m_radiusY(5) {
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsEllipseItem::~GraphicsEllipseItem() {

}

bool ot::GraphicsEllipseItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsEllipseItemCfg* cfg = dynamic_cast<ot::GraphicsEllipseItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_radiusX = cfg->radiusX();
	m_radiusY = cfg->radiusY();
	m_brush = ot::Painter2DFactory::brushFromPainter2D(cfg->backgroundPainter());
	m_pen.setWidth(cfg->border().top()); // ToDo: Add seperate borders on all 4 sides
	m_pen.setBrush(QBrush(ot::OTQtConverter::toQt(cfg->border().color())));
	m_pen.setColor(ot::OTQtConverter::toQt(cfg->border().color()));

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsEllipseItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsEllipseItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		//return QSizeF(m_radiusX * 2., m_radiusY * 2.);
		return this->handleGetGraphicsItemSizeHint(_hint, QSizeF(m_radiusX * 2., m_radiusY * 2.));
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	return _constrains;
};

QRectF ot::GraphicsEllipseItem::boundingRect(void) const {
	//return QRectF(QPointF(0., 0.), this->geometry().size());
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), QSizeF(m_radiusX * 2., m_radiusY * 2.)));
}

void ot::GraphicsEllipseItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QVariant ot::GraphicsEllipseItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return _value;
}

void ot::GraphicsEllipseItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

void ot::GraphicsEllipseItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsEllipseItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);
	_painter->drawEllipse(this->boundingRect().center(), m_radiusX, m_radiusY);
}

void ot::GraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsEllipseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsEllipseItem::setRadius(int _x, int _y) {
	this->prepareGeometryChange();
	m_radiusX = _x;
	m_radiusY = _y;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsTextItem::GraphicsTextItem()
{
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsTextItem::~GraphicsTextItem() {

}

bool ot::GraphicsTextItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsTextItemCfg* cfg = dynamic_cast<ot::GraphicsTextItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}
	
	this->prepareGeometryChange();

	QFont f = this->font();
	f.setPixelSize(cfg->textFont().size());
	f.setItalic(cfg->textFont().isItalic());
	f.setBold(cfg->textFont().isBold());


	this->setFont(f);
	this->setDefaultTextColor(ot::OTQtConverter::toQt(cfg->textColor()));
	this->setPlainText(QString::fromStdString(cfg->text()));

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsTextItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsTextItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
	{
		QFontMetrics m(this->font());
		//return QSizeF(m.width(this->toPlainText()), m.height());
		return this->handleGetGraphicsItemSizeHint(_hint, QSizeF(m.width(this->toPlainText()), m.height()));
	}
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	QFontMetrics m(this->font());

	//return QSizeF(m.width(this->toPlainText()), m.height());
	return this->handleGetGraphicsItemSizeHint(_hint, QSizeF(m.width(this->toPlainText()), m.height()));
};

QRectF ot::GraphicsTextItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QGraphicsTextItem::boundingRect());
	//return QGraphicsTextItem::boundingRect();
}

void ot::GraphicsTextItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsTextItem::paint(_painter, _opt, _widget);
}

void ot::GraphicsTextItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

void ot::GraphicsTextItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

void ot::GraphicsTextItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

QVariant ot::GraphicsTextItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return QGraphicsTextItem::itemChange(_change, _value);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsImageItem::GraphicsImageItem()
{
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsImageItem::~GraphicsImageItem() {

}

bool ot::GraphicsImageItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsImageItemCfg* cfg = dynamic_cast<ot::GraphicsImageItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}
	
	this->prepareGeometryChange();

	try {

		this->setPixmap(ot::IconManager::instance().getPixmap(QString::fromStdString(cfg->imagePath())));
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		return false;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
		return false;
	}
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsImageItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsImageItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		return this->handleGetGraphicsItemSizeHint(_hint, this->pixmap().size());
		//return QSizeF(this->pixmap().size());
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}

	return this->handleGetGraphicsItemSizeHint(_hint, this->pixmap().size());
	//return QSizeF(this->pixmap().size());
};

void ot::GraphicsImageItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QRectF ot::GraphicsImageItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QGraphicsPixmapItem::boundingRect());
	//return QGraphicsPixmapItem::boundingRect();
}

QVariant ot::GraphicsImageItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return QGraphicsPixmapItem::itemChange(_change, _value);
}

void ot::GraphicsImageItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsImageItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsImageItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsPixmapItem::paint(_painter, _opt, _widget);
}

void ot::GraphicsImageItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsLineItem::GraphicsLineItem()
{
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsLineItem::~GraphicsLineItem() {

}

bool ot::GraphicsLineItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	// Ignore, return false in case the setup is called to ensure this item wont be setup
	return false;
}

void ot::GraphicsLineItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsLineItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		return this->handleGetGraphicsItemSizeHint(_hint, this->boundingRect().size());
		//return this->boundingRect().size();
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	return this->handleGetGraphicsItemSizeHint(_hint, this->boundingRect().size());
	//return _constrains;
}

void ot::GraphicsLineItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	this->setPos(_rect.topLeft());
	QGraphicsLayoutItem::setGeometry(_rect);
}

QRectF ot::GraphicsLineItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), QGraphicsLineItem::boundingRect().size()));
	//return QGraphicsLineItem::boundingRect();
}

QVariant ot::GraphicsLineItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return QGraphicsLineItem::itemChange(_change, _value);
}

void ot::GraphicsLineItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsLineItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	// Ignore
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

std::string ot::GraphicsConnectionItem::buildKey(const std::string& _originUid, const std::string& _originItemName, const std::string& _destUid, const std::string& _destItemName) {
	return _originUid + "|" + _originItemName + "|" + _destUid + "|" + _destItemName;
}

ot::GraphicsConnectionItem::GraphicsConnectionItem() : m_origin(nullptr), m_dest(nullptr) {
	
}

ot::GraphicsConnectionItem::~GraphicsConnectionItem() {

}

void ot::GraphicsConnectionItem::connectItems(GraphicsItem* _origin, GraphicsItem* _dest) {
	OTAssertNullptr(_origin);
	OTAssertNullptr(_dest);
	otAssert(m_origin == nullptr, "Origin already set");
	otAssert(m_dest == nullptr, "Origin already set");
	m_origin = _origin;
	m_dest = _dest;
	m_origin->storeConnection(this);
	m_dest->storeConnection(this);
	this->updateConnection();
}

void ot::GraphicsConnectionItem::disconnectItems(void) {
	if (m_origin) {
		m_origin->forgetConnection(this);
		m_origin = nullptr;
	}
	if (m_dest) {
		m_dest->forgetConnection(this);
		m_dest = nullptr;
	}
}

void ot::GraphicsConnectionItem::updateConnection(void) {
	if (m_origin == nullptr || m_dest == nullptr) {
		OT_LOG_EA("Can not draw connection since to item were set");
		return;
	}
	
	QPointF orig = m_origin->getQGraphicsItem()->scenePos()  + m_origin->getQGraphicsItem()->boundingRect().center();
	QPointF dest = m_dest->getQGraphicsItem()->scenePos() + m_dest->getQGraphicsItem()->boundingRect().center();

	//OT_LOG_W("Updating coonection { O.X = " + std::to_string(orig.x()) + "; O.Y = " + std::to_string(orig.y()) + "; D.X = " + std::to_string(dest.x()) + "; D.Y = " + std::to_string(dest.y()) + " }");

	this->setLine(QLineF(orig, dest));
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsTextItem> textItem(OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsStackItem> stackItem(OT_SimpleFactoryJsonKeyValue_GraphicsStackItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsImageItem> imageItem(OT_SimpleFactoryJsonKeyValue_GraphicsImageItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsEllipseItem> elliItem(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItem> rectItem(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);

// Register at global key map (config -> item)
static ot::GlobalKeyMapRegistrar textItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::GlobalKeyMapRegistrar stackItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsStackItem);
static ot::GlobalKeyMapRegistrar imageItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsImageItem);
static ot::GlobalKeyMapRegistrar elliItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem);
static ot::GlobalKeyMapRegistrar rectItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);