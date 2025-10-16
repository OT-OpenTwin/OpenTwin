//! @file GraphicsGridLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsGridLayoutItem.h"

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsGridLayoutItem> glayItemRegistrar(ot::GraphicsGridLayoutItemCfg::className());

ot::GraphicsGridLayoutItem::GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem) 
	: QGraphicsGridLayout(_parentItem), GraphicsLayoutItem(new GraphicsGridLayoutItemCfg)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->createLayoutWrapper(this);
}

ot::GraphicsGridLayoutItem::~GraphicsGridLayoutItem() {
	for (int r = 0; r < this->rowCount(); r++) {
		for (int c = 0; c < this->columnCount(); c++) {
			GraphicsItem* element = dynamic_cast<GraphicsItem*>(this->itemAt(r, c));
			if (element) {
				element->setParentGraphicsItem(nullptr);
			}
		}
	}
}

bool ot::GraphicsGridLayoutItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	const GraphicsGridLayoutItemCfg* cfg = dynamic_cast<const GraphicsGridLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setBlockConfigurationNotifications(true);
	this->setContentsMargins(_cfg->getMargins().left(), _cfg->getMargins().top(), _cfg->getMargins().right(), _cfg->getMargins().bottom());

	// Create items
	int x = 0;
	for (auto r : cfg->getItems()) {
		int y = 0;
		for (auto c : r) {
			if (c) {
				ot::GraphicsItem* i = ot::GraphicsItemFactory::itemFromConfig(c);
				if (i == nullptr) {
					OT_LOG_EA("GraphicsFactory failed");
					this->setBlockConfigurationNotifications(false);
					return false;
				}
				i->setParentGraphicsItem(this);
				OTAssertNullptr(i->getQGraphicsLayoutItem());
				this->addItem(i->getQGraphicsLayoutItem(), x, y, QtFactory::toQAlignment(i->getGraphicsItemAlignment()));
			}
			y++;
		}
		x++;
	}

	// Setup stretches
	for (size_t r = 0; r < cfg->getRowStretch().size(); r++) {
		if (cfg->getRowStretch()[r] > 0) this->setRowStretchFactor(r, cfg->getRowStretch()[r]);
	}
	for (size_t c = 0; c < cfg->getColumnStretch().size(); c++) {
		if (cfg->getColumnStretch()[c] > 0) this->setColumnStretchFactor(c, cfg->getColumnStretch()[c]);
	}

	this->setMinimumSize(QtFactory::toQSize(_cfg->getMinimumSize()));
	this->setMaximumSize(QtFactory::toQSize(_cfg->getMaximumSize()));

	this->setBlockConfigurationNotifications(false);
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
