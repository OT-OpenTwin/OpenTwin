//! @file GraphicsItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTWidgets/GraphicsItem.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinCore/KeyMap.h"

#include <QtGui/qfont.h>
#include <QtWidgets/qgraphicsscene.h>

ot::GraphicsItem::GraphicsItem() : m_flags(GraphicsItem::NoFlags), m_group(nullptr) {
	
}

ot::GraphicsItem::~GraphicsItem() {
	
}

void ot::GraphicsItem::setGraphicsItemFlags(ot::GraphicsItem::GraphicsItemFlag _flags) {
	m_flags = _flags;
	this->graphicsItemFlagsChanged(_flags);
}

void ot::GraphicsItem::finalizeAsRootItem(QGraphicsScene* _scene) {
	otAssert(m_group == nullptr, "Group item already created");
	m_group = new QGraphicsItemGroup;
	this->finalizeItem(_scene, m_group, true);
	_scene->addItem(m_group);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsRectangularItem::GraphicsRectangularItem() : m_size(200, 100) {

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

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsTextItem::GraphicsTextItem() {

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

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsTextItem> textItem(OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItem> rectItem(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);

// Register at global key map (config -> item)
static ot::GlobalKeyMapRegistrar textItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::GlobalKeyMapRegistrar rectItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);