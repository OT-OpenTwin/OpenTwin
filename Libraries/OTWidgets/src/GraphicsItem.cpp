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

ot::GraphicsItem::GraphicsItem(bool _containerItem) : m_flags(GraphicsItem::NoFlags), m_drag(nullptr), m_parent(nullptr), m_isContainerItem(_containerItem), m_hasHover(false), m_scene(nullptr), m_uid(0) {}

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
	return true;
}

void ot::GraphicsItem::paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (m_hasHover && (m_flags & GraphicsItem::ItemIsConnectable)) _painter->fillRect(this->getGraphicsItemBoundingRect(), Qt::GlobalColor::green);
}

void ot::GraphicsItem::handleItemMoved(void) {
	for (auto c : m_connections) c->updateConnection();
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

ot::GraphicsStackItem::GraphicsStackItem() : GraphicsGroupItem(true), m_top(nullptr), m_bottom(nullptr) {

}

ot::GraphicsStackItem::~GraphicsStackItem() {
	if (m_top) delete m_top;
	if (m_bottom) delete m_bottom;
}

bool ot::GraphicsStackItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsStackItemCfg* cfg = dynamic_cast<ot::GraphicsStackItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}
	OTAssertNullptr(cfg->topItem());
	OTAssertNullptr(cfg->bottomItem());

	if (m_bottom) delete m_bottom;
	if (m_top) delete m_top;
	m_bottom = nullptr;
	m_top = nullptr;
	m_bottom = ot::GraphicsFactory::itemFromConfig(cfg->bottomItem());
	m_top = ot::GraphicsFactory::itemFromConfig(cfg->topItem());
	if (m_top == nullptr || m_bottom == nullptr) {
		OT_LOG_EA("Failed to create child item(s). Abort");
		if (m_bottom) delete m_bottom;
		if (m_top) delete m_top;
		m_bottom = nullptr;
		m_top = nullptr;
		return false;
	}
	m_bottom->setParentGraphicsItem(this);
	m_top->setParentGraphicsItem(this);

	this->addToGroup(m_bottom->getQGraphicsItem());
	this->addToGroup(m_top->getQGraphicsItem());

	return ot::GraphicsGroupItem::setupFromConfig(_cfg);
}

void ot::GraphicsStackItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	ot::GraphicsGroupItem::callPaint(_painter, _opt, _widget);
	if (m_bottom) m_bottom->callPaint(_painter, _opt, _widget);
	if (m_top) m_top->callPaint(_painter, _opt, _widget);
}

void ot::GraphicsStackItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	ot::GraphicsGroupItem::graphicsItemFlagsChanged(_flags);
	if (m_bottom) m_bottom->setGraphicsItemFlags(_flags);
	if (m_top) m_top->setGraphicsItemFlags(_flags);
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
	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());
	m_cornerRadius = cfg->cornerRadius();
	m_brush = ot::Painter2DFactory::brushFromPainter2D(cfg->backgroundPainter());
	m_pen.setWidth(cfg->border().top()); // ToDo: Add seperate borders on all 4 sides
	m_pen.setBrush(QBrush(ot::OTQtConverter::toQt(cfg->border().color())));
	m_pen.setColor(ot::OTQtConverter::toQt(cfg->border().color()));
	
	//OT_LOG_W("Orig: " + std::to_string(cfg->border().color().r()) + "; " + std::to_string(cfg->border().color().g()) + "; " + std::to_string(cfg->border().color().b()) + "; " + std::to_string(cfg->border().color().a()));
	//OT_LOG_W(": " + std::to_string(m_pen.color().red()) + "; " + std::to_string(m_pen.color().green()) + "; " + std::to_string(m_pen.color().blue()) + "; " + std::to_string(m_pen.color().alpha()));

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

QRectF ot::GraphicsRectangularItem::boundingRect(void) const {
	return QRectF(this->pos(), m_size);
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
	
	QFont f = this->font();
	f.setPixelSize(cfg->textFont().size());
	f.setItalic(cfg->textFont().isItalic());
	f.setBold(cfg->textFont().isBold());
	this->setFont(f);

	this->setDefaultTextColor(ot::OTQtConverter::toQt(cfg->textColor()));
	
	//this->setDefaultTextColor(QColor(cfg->textColor().r(), cfg->textColor().g(), cfg->textColor().b(), cfg->textColor().a()));

	this->setPlainText(QString::fromStdString(cfg->text()));

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

QSizeF ot::GraphicsTextItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->boundingRect().size();
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

void ot::GraphicsTextItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsTextItem::paint(_painter, _opt, _widget);
}

void ot::GraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleItemClickEvent(_event, boundingRect());
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
	
	return this->boundingRect().size();
}

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
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItem> rectItem(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);

// Register at global key map (config -> item)
static ot::GlobalKeyMapRegistrar textItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::GlobalKeyMapRegistrar stackItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsStackItem);
static ot::GlobalKeyMapRegistrar imageItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsImageItem);
static ot::GlobalKeyMapRegistrar rectItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);