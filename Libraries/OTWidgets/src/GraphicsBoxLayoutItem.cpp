//! @file GraphicsBoxLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsBoxLayoutItemCfg.h"
#include "OTWidgets/GraphicsBoxLayoutItem.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/OTQtConverter.h"

ot::GraphicsBoxLayoutItem::GraphicsBoxLayoutItem(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem) : QGraphicsLinearLayout(_orientation, _parentItem)
{
	this->createLayoutWrapper(this);
}

bool ot::GraphicsBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsBoxLayoutItemCfg* cfg = dynamic_cast<ot::GraphicsBoxLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setContentsMargins(_cfg->margins().left(), _cfg->margins().top(), _cfg->margins().right(), _cfg->margins().bottom());

	for (auto itm : cfg->items()) {
		if (itm.first) {
			ot::GraphicsItem* i = ot::GraphicsFactory::itemFromConfig(itm.first);
			if (i == nullptr) {
				OT_LOG_EA("GraphicsFactory failed");
				return false;
			}
			i->setParentGraphicsItem(this);
			OTAssertNullptr(i->getQGraphicsLayoutItem());
			this->addItem(i->getQGraphicsLayoutItem());
			if (itm.second > 0) this->setStretchFactor(i->getQGraphicsLayoutItem(), itm.second);
			this->setAlignment(i->getQGraphicsLayoutItem(), ot::OTQtConverter::toQt(i->graphicsItemAlignment()));
		}
		else {
			this->addStretch(itm.second);
		}
	}

	return GraphicsLayoutItem::setupFromConfig(_cfg);
}

void ot::GraphicsBoxLayoutItem::getAllItems(std::list<QGraphicsLayoutItem*>& _items) const {
	for (int i = 0; i < this->count(); i++) {
		if (this->itemAt(i)) _items.push_back(this->itemAt(i));
	}
}

