//! @file GraphicsBoxLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsBoxLayoutItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsBoxLayoutItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

ot::GraphicsBoxLayoutItem::GraphicsBoxLayoutItem(Qt::Orientation _orientation, GraphicsItemCfg* _configuration, QGraphicsLayoutItem* _parentItem) 
	: QGraphicsLinearLayout(_orientation, _parentItem), GraphicsLayoutItem(_configuration)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->createLayoutWrapper(this);
}

ot::GraphicsBoxLayoutItem::~GraphicsBoxLayoutItem() {
	for (int i = 0; i < this->count(); i++) {
		if (this->itemAt(i)) {
			GraphicsItem* element = dynamic_cast<GraphicsItem*>(this->itemAt(i));
			if (element) {
				element->setParentGraphicsItem(nullptr);
			}
		}
	}
}

bool ot::GraphicsBoxLayoutItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	const GraphicsBoxLayoutItemCfg* cfg = dynamic_cast<const GraphicsBoxLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setBlockConfigurationNotifications(true);
	this->setContentsMargins(_cfg->getMargins().left(), _cfg->getMargins().top(), _cfg->getMargins().right(), _cfg->getMargins().bottom());

	for (auto itm : cfg->items()) {
		if (itm.first) {
			ot::GraphicsItem* i = ot::GraphicsItemFactory::itemFromConfig(itm.first);
			if (i == nullptr) {
				OT_LOG_EA("GraphicsFactory failed");
				this->setBlockConfigurationNotifications(false);
				return false;
			}
			i->setParentGraphicsItem(this);
			OTAssertNullptr(i->getQGraphicsLayoutItem());
			this->addItem(i->getQGraphicsLayoutItem());
			if (itm.second > 0) this->setStretchFactor(i->getQGraphicsLayoutItem(), itm.second);
			this->setAlignment(i->getQGraphicsLayoutItem(), QtFactory::toQAlignment(i->getGraphicsItemAlignment()));
		}
		else {
			this->addStretch(itm.second);
		}
	}

	this->setMinimumSize(QtFactory::toQSize(_cfg->getMinimumSize()));
	this->setMaximumSize(QtFactory::toQSize(_cfg->getMaximumSize()));

	this->setBlockConfigurationNotifications(false);

	return GraphicsLayoutItem::setupFromConfig(_cfg);
}

void ot::GraphicsBoxLayoutItem::getAllItems(std::list<QGraphicsLayoutItem*>& _items) const {
	for (int i = 0; i < this->count(); i++) {
		if (this->itemAt(i)) {
			_items.push_back(this->itemAt(i));
		}
	}
}

QSizeF ot::GraphicsBoxLayoutItem::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}