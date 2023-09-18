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

void ot::GraphicsItemDrag::queue(QWidget* _widget, const QRectF& _rect) {
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
		mimeData->setData(OT_GRAPHICSITEM_MIMETYPE_Configuration, QByteArray::fromStdString(m_owner->configuration()));

		// Create drag
		drag.setMimeData(mimeData);

		// Create preview
		QPixmap prev(m_owner->getGraphicsItemBoundingRect().size().toSize());
		QPainter p(&prev);
		QStyleOptionGraphicsItem opt;
		p.fillRect(QRect(QPoint(0, 0), m_owner->getGraphicsItemBoundingRect().size().toSize()), Qt::gray);

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

ot::GraphicsItem::GraphicsItem(bool _containerItem) : m_flags(GraphicsItem::NoFlags), m_drag(nullptr), m_parent(nullptr), 
	m_isContainerItem(_containerItem), m_hasHover(false), m_scene(nullptr), m_uid(0), m_alignment(ot::AlignCenter), m_requestedSize(10., 10.)
{

}

ot::GraphicsItem::~GraphicsItem() {
	if (m_drag) delete m_drag;
}

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags) {
	m_flags = _flags;
	this->graphicsItemFlagsChanged(m_flags);
}

ot::GraphicsItem* ot::GraphicsItem::getRootItem(void) {
	if (m_parent) return m_parent->getRootItem();
	return this;
}

void ot::GraphicsItem::handleItemClickEvent(QGraphicsSceneMouseEvent* _event, const QRectF& _rect) {
	if (m_parent) {
		m_parent->handleItemClickEvent(_event, _rect);
	}
	else if (m_flags & ot::GraphicsItem::ItemPreviewContext) {
		// Start drag since its a preview item
		otAssert(!m_configuration.empty(), "No configuration set");

		if (_event->button() == Qt::LeftButton) {
			if (m_drag == nullptr) {
				m_drag = new GraphicsItemDrag(this);
			}
			m_drag->queue(_event->widget(), _rect);
		}
	}
	else if (m_flags & ot::GraphicsItem::ItemNetworkContext) {
		if (m_flags & ot::GraphicsItem::ItemIsConnectable) {
			OTAssertNullptr(m_scene); // Ensure the finalizeItem() method calls setGraphicsScene()
			m_scene->startConnection(this);
		}
	}
}

bool ot::GraphicsItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	if (_cfg->graphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable) { m_flags |= GraphicsItem::ItemIsConnectable; }
	m_name = _cfg->name();
	m_alignment = _cfg->alignment();
	return true;
}

void ot::GraphicsItem::paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (m_hasHover && (m_flags & GraphicsItem::ItemIsConnectable)) _painter->fillRect(this->getGraphicsItemBoundingRect(), Qt::GlobalColor::green);
}

void ot::GraphicsItem::handleItemMoved(void) {
	for (auto c : m_connections) c->updateConnection();
	this->raiseEvent(ot::GraphicsItem::Resized); //ToDo: Seperate function
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

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Helper

QRectF ot::GraphicsItem::calculateDrawRect(const QRectF& _rect) const {
	// Size
	QSizeF s(_rect.size());
	if (s.width() > m_requestedSize.width()) { s.setWidth(m_requestedSize.width()); }
	if (s.height() > m_requestedSize.height()) { s.setHeight(m_requestedSize.height()); }

	// Position
	QPointF p; // Top left coord
	switch (m_alignment)
	{
	case ot::AlignCenter:
		p.setX(_rect.x() + ((_rect.width() / 2.) - (s.width() / 2.)));
		p.setY(_rect.y() + ((_rect.height() / 2.) - (s.height() / 2.)));
		break;
	case ot::AlignTop:
		p.setX(_rect.x() + ((_rect.width() / 2.) - (s.width() / 2.)));
		p.setY(_rect.y());
		break;
	case ot::AlignTopRight:
		p.setX(_rect.x() + (_rect.width() - s.width()));
		p.setY(_rect.y());
		break;
	case ot::AlignRight:
		p.setX(_rect.x() + (_rect.width() - s.width()));
		p.setY(_rect.y() + ((_rect.height() / 2.) - (s.height() / 2.)));
		break;
	case ot::AlignBottomRight:
		p.setX(_rect.x() + (_rect.width() - s.width()));
		p.setY(_rect.y() + (_rect.height() - s.height()));
		break;
	case ot::AlignBottom:
		p.setX(_rect.x() + ((_rect.width() / 2.) - (s.width() / 2.)));
		p.setY(_rect.y() + (_rect.height() - s.height()));
		break;
	case ot::AlignBottomLeft:
		p.setX(_rect.x());
		p.setY(_rect.y() + (_rect.height() - s.height()));
		break;
	case ot::AlignLeft:
		p.setX(_rect.x());
		p.setY(_rect.y() + ((_rect.height() / 2.) - (s.height() / 2.)));
	case ot::AlignTopLeft:
		p = _rect.topLeft();
		break;
	default:
		OT_LOG_EA("Unknown alignment provided");
		break;
	}

	return QRectF(p, s);
}

void ot::GraphicsItem::raiseEvent(ot::GraphicsItem::GraphicsItemEvent _event) {
	for (auto itm : m_eventHandler) {
		itm->graphicsItemEventHandler(this, _event);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsGroupItem::GraphicsGroupItem(bool _containerItem) : ot::GraphicsItem(_containerItem) {
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsGroupItem::~GraphicsGroupItem() {

}

bool ot::GraphicsGroupItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

QSizeF ot::GraphicsGroupItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->boundingRect().size();
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
	GraphicsItem::handleItemClickEvent(_event, boundingRect());
	QGraphicsItemGroup::mousePressEvent(_event);
}

void ot::GraphicsGroupItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {

	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsGroupItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsItemGroup::paint(_painter, _opt, _widget);
}

QRectF ot::GraphicsGroupItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

QPointF ot::GraphicsGroupItem::getGraphicsItemScenePos(void) const {
	return this->scenePos();
}

void ot::GraphicsGroupItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsStackItem::GraphicsStackItem() : GraphicsGroupItem(true) {

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
					//i->addGraphicsItemEventHandler(this);
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
	//for (auto itm : m_items) itm.item->callPaint(_painter, _opt, _widget);
}

void ot::GraphicsStackItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	ot::GraphicsGroupItem::graphicsItemFlagsChanged(_flags);
}

void ot::GraphicsStackItem::graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) {
	OTAssertNullptr(_sender);
	return;
	if (_event == ot::GraphicsItem::Resized) {
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

		for (auto itm : m_items) {
			if (itm.item != _sender && !itm.isMaster) {
				itm.item->setGraphicsItemRequestedSize(mas->getQGraphicsItem()->boundingRect().size());
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

ot::GraphicsRectangularItem::GraphicsRectangularItem() : ot::GraphicsItem(false), m_size(10, 10), m_cornerRadius(0) {
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

QSizeF ot::GraphicsRectangularItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		return m_size;
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	return _constrains;
};

QRectF ot::GraphicsRectangularItem::boundingRect(void) const {
	return QRectF(QPointF(0., 0.), this->geometry().size());
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

QRectF ot::GraphicsRectangularItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

QPointF ot::GraphicsRectangularItem::getGraphicsItemScenePos(void) const {
	return this->scenePos();
}

void ot::GraphicsRectangularItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleItemClickEvent(_event, boundingRect());
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsRectangularItem::setGraphicsItemRequestedSize(const QSizeF& _size) {
	if (_size == m_size) return;
	this->prepareGeometryChange();
	ot::GraphicsItem::setGraphicsItemRequestedSize(_size);
}

void ot::GraphicsRectangularItem::setRectangleSize(const QSizeF& _size) {
	m_size = _size;
	this->setGeometry(QRectF(this->pos(), m_size));
};

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsEllipseItem::GraphicsEllipseItem() : ot::GraphicsItem(false), m_radiusX(5), m_radiusY(5) {
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

QSizeF ot::GraphicsEllipseItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		return QSizeF(m_radiusX * 2., m_radiusY * 2.);
		return QSizeF(m_radiusX * 2., m_radiusY * 2.);
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	return _constrains;
};

QRectF ot::GraphicsEllipseItem::boundingRect(void) const {
	return QRectF(QPointF(0., 0.), this->geometry().size());
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

QRectF ot::GraphicsEllipseItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

QPointF ot::GraphicsEllipseItem::getGraphicsItemScenePos(void) const {
	return this->scenePos();
}

void ot::GraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleItemClickEvent(_event, boundingRect());
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsEllipseItem::setRadius(int _x, int _y) {
	this->prepareGeometryChange();
	m_radiusX = _x;
	m_radiusY = _y;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsTextItem::GraphicsTextItem() : ot::GraphicsItem(false) {
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

QSizeF ot::GraphicsTextItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
	{
		QFontMetrics m(this->font());
		return QSizeF(m.width(this->toPlainText()), m.height());
	}
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	return _constrains;
};

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

void ot::GraphicsTextItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsTextItem::paint(_painter, _opt, _widget);
}

void ot::GraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleItemClickEvent(_event, boundingRect());
	QGraphicsItem::mousePressEvent(_event);
}

QVariant ot::GraphicsTextItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return QGraphicsTextItem::itemChange(_change, _value);
}

QRectF ot::GraphicsTextItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

QPointF ot::GraphicsTextItem::getGraphicsItemScenePos(void) const {
	return this->scenePos();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsImageItem::GraphicsImageItem() : ot::GraphicsItem(false) {
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

QSizeF ot::GraphicsImageItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		return QSizeF(this->pixmap().size());
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	return _constrains;
};

void ot::GraphicsImageItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QVariant ot::GraphicsImageItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return QGraphicsPixmapItem::itemChange(_change, _value);
}

void ot::GraphicsImageItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleItemClickEvent(_event, boundingRect());
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsImageItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsImageItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsPixmapItem::paint(_painter, _opt, _widget);
}

QRectF ot::GraphicsImageItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

QPointF ot::GraphicsImageItem::getGraphicsItemScenePos(void) const {
	return this->scenePos();
}

void ot::GraphicsImageItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsPathItem::GraphicsPathItem() : ot::GraphicsItem(false) {
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsPathItem::~GraphicsPathItem() {

}

void ot::GraphicsPathItem::setPathPoints(const QPointF& _origin, const QPointF& _dest) {
	m_origin = _origin;
	m_dest = _dest;

	QPainterPath pth(m_origin);
	pth.lineTo(m_dest);

	this->setPath(pth);
}

bool ot::GraphicsPathItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	// Ignore, return false in case the setup is called to ensure this item wont be setup
	return false;
}

QVariant ot::GraphicsPathItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->handleItemMoved();
	}
	return QGraphicsPathItem::itemChange(_change, _value);
}

void ot::GraphicsPathItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

QRectF ot::GraphicsPathItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

QPointF ot::GraphicsPathItem::getGraphicsItemScenePos(void) const {
	return this->scenePos();
}

void ot::GraphicsPathItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	// Ignore
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

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

void ot::GraphicsConnectionItem::updateConnection(void) {
	if (m_origin == nullptr || m_dest == nullptr) {
		OT_LOG_EA("Can not draw connection since to item were set");
		return;
	}
	
	this->setPathPoints(
		m_origin->getGraphicsItemScenePos() + m_origin->getGraphicsItemBoundingRect().center(),
		m_dest->getGraphicsItemScenePos() + m_dest->getGraphicsItemBoundingRect().center()
	);
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