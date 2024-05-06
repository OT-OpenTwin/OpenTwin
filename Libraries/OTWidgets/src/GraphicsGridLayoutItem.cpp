//! @file GraphicsGridLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsGridLayoutItem.h"

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsGridLayoutItem> glayItemRegistrar(OT_FactoryKey_GraphicsGridLayoutItem);

ot::GraphicsGridLayoutItem::GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem) : QGraphicsGridLayout(_parentItem)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->createLayoutWrapper(this);
}

bool ot::GraphicsGridLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsGridLayoutItemCfg* cfg = dynamic_cast<ot::GraphicsGridLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setContentsMargins(_cfg->margins().left(), _cfg->margins().top(), _cfg->margins().right(), _cfg->margins().bottom());

	// Create items
	int x = 0;
	for (auto r : cfg->items()) {
		int y = 0;
		for (auto c : r) {
			if (c) {
				ot::GraphicsItem* i = ot::GraphicsItemFactory::instance().itemFromConfig(c);
				if (i == nullptr) {
					OT_LOG_EA("GraphicsFactory failed");
					return false;
				}
				i->setParentGraphicsItem(this);
				OTAssertNullptr(i->getQGraphicsLayoutItem());
				this->addItem(i->getQGraphicsLayoutItem(), x, y, QtFactory::toQt(i->graphicsItemAlignment()));
			}
			y++;
		}
		x++;
	}

	// Setup stretches
	for (size_t r = 0; r < cfg->rowStretch().size(); r++) {
		if (cfg->rowStretch()[r] > 0) this->setRowStretchFactor(r, cfg->rowStretch()[r]);
	}
	for (size_t c = 0; c < cfg->columnStretch().size(); c++) {
		if (cfg->columnStretch()[c] > 0) this->setColumnStretchFactor(c, cfg->columnStretch()[c]);
	}

	this->setMinimumSize(QtFactory::toQt(_cfg->minimumSize()));
	this->setMaximumSize(QtFactory::toQt(_cfg->maximumSize()));

	return GraphicsLayoutItem::setupFromConfig(_cfg);
}

void ot::GraphicsGridLayoutItem::getAllItems(std::list<QGraphicsLayoutItem*>& _items) const {
	for (int r = 0; r < this->rowCount(); r++) {
		for (int c = 0; c < this->columnCount(); c++) {
			if (this->itemAt(r, c)) _items.push_back(this->itemAt(r, c));
		}
	}
}

QSizeF ot::GraphicsGridLayoutItem::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}
