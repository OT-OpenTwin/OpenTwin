//! @file GraphicsItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTWidgets/GraphicsItem.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinCore/KeyMap.h"

#include <QtCore/qmimedata.h>
#include <QtGui/qdrag.h>
#include <QtGui/qfont.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qstyleoption.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qwidget.h>

ot::GraphicsItem::GraphicsItem(bool _isLayout) : m_flags(GraphicsItem::NoFlags), m_group(nullptr), m_isLayout(_isLayout) {
	
}

ot::GraphicsItem::~GraphicsItem() {
	
}

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags) {
	m_flags = _flags;
	if (m_group) {
		m_group->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
		m_group->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
	}
	this->graphicsItemFlagsChanged(_flags);
}

void ot::GraphicsItem::finalizeAsRootItem(QGraphicsScene* _scene) {
	otAssert(m_group == nullptr, "Group item already created");
	if (m_isLayout) {
		m_group = new QGraphicsItemGroup;
		this->finalizeItem(_scene, m_group, true);
		_scene->addItem(m_group);
	}
	else {
		this->finalizeItem(_scene, m_group, true);
	}
	
}

void ot::GraphicsItem::handleItemClickEvent(QGraphicsSceneMouseEvent* _event, const QRectF& _rect) {
	if (m_flags & ot::GraphicsItem::ItemPreviewContext) {
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
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsRectangularItem::GraphicsRectangularItem() : ot::GraphicsItem(false), m_size(200, 100) {

}

ot::GraphicsRectangularItem::~GraphicsRectangularItem() {

}

bool ot::GraphicsRectangularItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsRectangularItemCfg* cfg = dynamic_cast<ot::GraphicsRectangularItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}
	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());
	setRect(0., 0., m_size.width(), m_size.height());

	return true;
}

void ot::GraphicsRectangularItem::setGeometry(const QRectF& rect) {
	setPos(rect.topLeft());
}

void ot::GraphicsRectangularItem::finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) {
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

	return true;
}

void ot::GraphicsTextItem::setGeometry(const QRectF& rect) {
	setPos(rect.topLeft());
}

void ot::GraphicsTextItem::finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) {
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

void ot::GraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleItemClickEvent(_event, boundingRect());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsTextItem> textItem(OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItem> rectItem(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);

// Register at global key map (config -> item)
static ot::GlobalKeyMapRegistrar textItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::GlobalKeyMapRegistrar rectItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);