//! @file GraphicsItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTWidgets/GraphicsItem.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinCore/KeyMap.h"

#include <QtGui/qfont.h>

ot::GraphicsItem::GraphicsItem() {
	
}

ot::GraphicsItem::~GraphicsItem() {
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsRectangularItem::GraphicsRectangularItem() {

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

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsTextItem> textItem(OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItem> rectItem(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);

// Register at global key map (config -> item)
static ot::GlobalKeyMapRegistrar textItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::GlobalKeyMapRegistrar rectItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);