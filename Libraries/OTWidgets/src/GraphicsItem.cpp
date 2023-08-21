//! @file GraphicsItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/IconManager.h"
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

ot::GraphicsItem::GraphicsItem(bool _containerItem) : m_flags(GraphicsItem::NoFlags), m_parent(nullptr), m_group(nullptr), m_isContainerItem(_containerItem), m_hasHover(false), m_scene(nullptr) {}

ot::GraphicsItem::~GraphicsItem() {
	
}

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags) {
	m_flags = _flags;
	if (m_group) {
		m_group->setGraphicsItemFlags(m_flags);
	}
	else {
		this->graphicsItemFlagsChanged(m_flags);
	}
}

void ot::GraphicsItem::finalizeAsRootItem(GraphicsScene* _scene) {
	otAssert(m_group == nullptr, "Group item already created");

	this->setGraphicsScene(_scene);

	if (m_isContainerItem) {
		m_group = new GraphicsGroupItem;
		m_group->setParentGraphicsItem(this);
		
		this->finalizeItem(_scene, m_group, true);
		m_group->finalizeItem(_scene, nullptr, false);
	}
	else {
		this->finalizeItem(_scene, m_group, true);
	}	
}

void ot::GraphicsItem::handleItemClickEvent(QGraphicsSceneMouseEvent* _event, const QRectF& _rect) {
	if (m_parent) {
		m_parent->handleItemClickEvent(_event, _rect);
	}
	else if (m_flags & ot::GraphicsItem::ItemPreviewContext) {
		// Start drag since its a preview item
		otAssert(!m_configuration.empty(), "No configuration set");

		if (_event->button() == Qt::LeftButton) {
			// Add configuration to mime data
			QMimeData* mimeData = new QMimeData;
			mimeData->setText("OT_BLOCK");
			mimeData->setData(OT_GRAPHICSITEM_MIMETYPE_Configuration, QByteArray::fromStdString(m_configuration));
			
			// Create drag
			QDrag* drag = new QDrag(_event->widget());
			drag->setMimeData(mimeData);

			// Create preview
			QPixmap prev(_rect.size().toSize());
			QPainter p(&prev);
			QStyleOptionGraphicsItem opt;
			p.fillRect(QRect(QPoint(0, 0), _rect.size().toSize()), Qt::gray);

			// Paint
			this->callPaint(&p, &opt, _event->widget());

			// Run drag
			drag->setPixmap(prev);
			drag->exec();
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
	if (_cfg->graphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable) m_flags |= GraphicsItem::ItemIsConnectable;
	return true;
}

void ot::GraphicsItem::paintGeneralGraphics(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (m_hasHover && (m_flags & GraphicsItem::ItemIsConnectable)) _painter->fillRect(this->getGraphicsItemBoundingRect(), Qt::GlobalColor::green);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsGroupItem::GraphicsGroupItem() : ot::GraphicsItem(true) {

}

ot::GraphicsGroupItem::~GraphicsGroupItem() {

}

bool ot::GraphicsGroupItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

QSizeF ot::GraphicsGroupItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->contentsRect().size();
}

void ot::GraphicsGroupItem::setGeometry(const QRectF& _rect) {
	setPos(_rect.topLeft());
}

void ot::GraphicsGroupItem::finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group, bool _isRoot) {
	this->setGraphicsScene(_scene);
	_scene->addItem(this);
	if (_group) _group->addToGroup(this);
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

void ot::GraphicsGroupItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsStackItem::GraphicsStackItem() : m_top(nullptr), m_bottom(nullptr) {

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
	return ot::GraphicsGroupItem::setupFromConfig(_cfg);
}

void ot::GraphicsStackItem::finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group, bool _isRoot) {
	ot::GraphicsGroupItem::finalizeItem(_scene, _group, _isRoot);
	if (m_bottom) {
		m_bottom->finalizeItem(_scene, _group, false);
	}
	if (m_top) {
		m_top->finalizeItem(_scene, _group, false);
	}
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

ot::GraphicsRectangularItem::GraphicsRectangularItem() : ot::GraphicsItem(false), m_size(200, 100) {

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
	setRect(0., 0., m_size.width(), m_size.height());

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsRectangularItem::setGeometry(const QRectF& rect) {
	setPos(rect.topLeft());
}

void ot::GraphicsRectangularItem::finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group, bool _isRoot) {
	this->setGraphicsScene(_scene);

	_scene->addItem(this);
	if (_group) _group->addToGroup(this);
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
	QGraphicsRectItem::paint(_painter, _opt, _widget);
}

QRectF ot::GraphicsRectangularItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

void ot::GraphicsRectangularItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleItemClickEvent(_event, boundingRect());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsTextItem::GraphicsTextItem() : ot::GraphicsItem(false) {

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
	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());
	
	QFont f = this->font();
	f.setPixelSize(cfg->textFont().size());
	f.setItalic(cfg->textFont().isItalic());
	f.setBold(cfg->textFont().isBold());
	this->setFont(f);
	
	//this->setDefaultTextColor(QColor(cfg->textColor().r(), cfg->textColor().g(), cfg->textColor().b(), cfg->textColor().a()));

	this->setPlainText(QString::fromStdString(cfg->text()));

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsTextItem::setGeometry(const QRectF& rect) {
	setPos(rect.topLeft());
}

void ot::GraphicsTextItem::finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group, bool _isRoot) {
	this->setGraphicsScene(_scene);

	_scene->addItem(this);
	if (_group) _group->addToGroup(this);
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

QRectF ot::GraphicsTextItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsImageItem::GraphicsImageItem() : ot::GraphicsItem(false) {

}

ot::GraphicsImageItem::~GraphicsImageItem() {

}

bool ot::GraphicsImageItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsImageItemCfg* cfg = dynamic_cast<ot::GraphicsImageItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}
	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());

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

void ot::GraphicsImageItem::setGeometry(const QRectF& rect) {
	setPos(rect.topLeft());
}

void ot::GraphicsImageItem::finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group, bool _isRoot) {
	this->setGraphicsScene(_scene);

	_scene->addItem(this);
	if (_group) _group->addToGroup(this);
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

void ot::GraphicsImageItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsPathItem::GraphicsPathItem() {

}

ot::GraphicsPathItem::~GraphicsPathItem() {

}

void ot::GraphicsPathItem::setPathPoints(const QPointF& _origin, const QPointF& _dest) {
	m_origin = _origin;
	m_dest = _dest;

	QPainterPath pth(m_origin);
	pth.lineTo(m_dest);

	this->setPath(pth);
	this->update();
}

bool ot::GraphicsPathItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	// Ignore, return false in case the setup is called to ensure this item wont be setup
	return false;
}

void ot::GraphicsPathItem::finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group, bool _isRoot) {
	if (_group) _group->addToGroup(this);
	_scene->addItem(this);
}

void ot::GraphicsPathItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

QRectF ot::GraphicsPathItem::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

void ot::GraphicsPathItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	// Ignore
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