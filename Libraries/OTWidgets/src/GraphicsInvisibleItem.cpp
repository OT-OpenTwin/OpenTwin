//! @file GraphicsInvisibleItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsInvisibleItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsInvisibleItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsInvisibleItem> invisItemRegistrar(OT_FactoryKey_GraphicsInvisibleItem);

ot::GraphicsInvisibleItem::GraphicsInvisibleItem()
	: ot::CustomGraphicsItem(new GraphicsInvisibleItemCfg)
{
	this->setAcceptHoverEvents(false);
}

ot::GraphicsInvisibleItem::~GraphicsInvisibleItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsInvisibleItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);

	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->setBlockConfigurationNotifications(true);
	this->updateItemGeometry();
	this->setBlockConfigurationNotifications(false);

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsInvisibleItem::getPreferredGraphicsItemSize(void) const {
	return QtFactory::toQSize(this->getItemConfiguration<GraphicsInvisibleItemCfg>()->getSize());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsInvisibleItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	QRectF br = this->boundingRect();
	if (br.size().isNull()) return;

	const GraphicsInvisibleItemCfg* cfg = this->getItemConfiguration<GraphicsInvisibleItemCfg>();

	if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState) {
		if ((this->getGraphicsElementState() & GraphicsElement::SelectedState) && !(this->getGraphicsElementState() & GraphicsElement::HoverState)) {
			PenFCfg selectedOutline(1., GraphicsItem::createSelectionBorderPainter());
			_painter->setPen(QtFactory::toQPen(selectedOutline));
			_painter->drawEllipse(br);
		}
		else if (this->getGraphicsElementState() & GraphicsElement::HoverState) {
			PenFCfg hoverOutline(1., GraphicsItem::createHoverBorderPainter());
			_painter->setPen(QtFactory::toQPen(hoverOutline));
			_painter->drawEllipse(br);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsInvisibleItem::setItemSize(const Size2DD& _size) {
	GraphicsInvisibleItemCfg* cfg = this->getItemConfiguration<GraphicsInvisibleItemCfg>();
	cfg->setSize(_size);

	this->updateItemGeometry();
	this->raiseEvent(GraphicsItem::ItemResized);
}

const ot::Size2DD& ot::GraphicsInvisibleItem::getItemSize(void) const {
	return this->getItemConfiguration<GraphicsInvisibleItemCfg>()->getSize();
}
