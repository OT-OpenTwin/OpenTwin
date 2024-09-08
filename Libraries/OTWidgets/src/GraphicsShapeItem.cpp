//! @file GraphicsShapeItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsShapeItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsShapeItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsShapeItem> polyItemRegistrar(OT_FactoryKey_GraphicsShapeItem);

ot::GraphicsShapeItem::GraphicsShapeItem()
	: ot::CustomGraphicsItem(new GraphicsShapeItemCfg)
{

}

ot::GraphicsShapeItem::~GraphicsShapeItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsShapeItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	const GraphicsShapeItemCfg* cfg = dynamic_cast<const GraphicsShapeItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setBlockConfigurationNotifications(true);
	this->updateItemGeometry();
	this->setBlockConfigurationNotifications(false);

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsShapeItem::getPreferredGraphicsItemSize(void) const {
	return QtFactory::toQPainterPath(this->getItemConfiguration<GraphicsShapeItemCfg>()->getOutlinePath()).boundingRect().size();
}

void ot::GraphicsShapeItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsShapeItemCfg* cfg = this->getItemConfiguration<GraphicsShapeItemCfg>();

	QPen borderPen = QtFactory::toQPen(cfg->getOutline());

	if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState) {
		if ((this->getGraphicsElementState() & GraphicsElement::SelectedState) && !(this->getGraphicsElementState() & GraphicsElement::HoverState)) {
			Painter2D* newPainter = GraphicsItem::createSelectionBorderPainter();
			borderPen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
		else if (this->getGraphicsElementState() & GraphicsElement::HoverState) {
			Painter2D* newPainter = GraphicsItem::createHoverBorderPainter();
			borderPen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
	}

	_painter->setPen(borderPen);

	QPainterPath painterPath = QtFactory::toQPainterPath(cfg->getOutlinePath());

	if (cfg->getFillShape()) {
		_painter->fillPath(painterPath, QtFactory::toQBrush(cfg->getBackgroundPainter()));
	}
	
	_painter->drawPath(painterPath);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsShapeItem::setBackgroundPainter(Painter2D* _painter) {
	this->getItemConfiguration<GraphicsShapeItemCfg>()->setBackgroundPainter(_painter);
}

const ot::Painter2D* ot::GraphicsShapeItem::getBackgroundPainter(void) const {
	return this->getItemConfiguration<GraphicsShapeItemCfg>()->getBackgroundPainter();
}

void ot::GraphicsShapeItem::setOutline(const OutlineF& _outline) {
	this->getItemConfiguration<GraphicsShapeItemCfg>()->setOutline(_outline);
}

const ot::OutlineF& ot::GraphicsShapeItem::getOutline(void) const {
	return this->getItemConfiguration<GraphicsShapeItemCfg>()->getOutline();
}

void ot::GraphicsShapeItem::setFillShape(bool _fill) {
	this->getItemConfiguration<GraphicsShapeItemCfg>()->setFillShape(_fill);
}

bool ot::GraphicsShapeItem::getFillShape(void) const {
	return this->getItemConfiguration<GraphicsShapeItemCfg>()->getFillShape();
}

void ot::GraphicsShapeItem::setOutlinePath(const Path2DF& _path) {
	this->getItemConfiguration<GraphicsShapeItemCfg>()->setOutlinePath(_path);
}

const ot::Path2DF& ot::GraphicsShapeItem::getOutlinePath(void) const {
	return this->getItemConfiguration<GraphicsShapeItemCfg>()->getOutlinePath();
}