//! @file GraphicsEllipseItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsEllipseItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsEllipseItem> elliItemRegistrar(OT_FactoryKey_GraphicsEllipseItem);

ot::GraphicsEllipseItem::GraphicsEllipseItem()
	: ot::CustomGraphicsItem(new GraphicsEllipseItemCfg)
{}

ot::GraphicsEllipseItem::~GraphicsEllipseItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsEllipseItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);

	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->setBlockConfigurationNotifications(true);
	this->updateItemGeometry();
	this->setBlockConfigurationNotifications(false);

	return true;
}

QMarginsF ot::GraphicsEllipseItem::getOutlineMargins(void) const {
	const GraphicsEllipseItemCfg* cfg = this->getItemConfiguration<GraphicsEllipseItemCfg>();
	OTAssertNullptr(cfg);
	double margs = cfg->getOutline().getWidth() / 2.;
	return QMarginsF(margs, margs, margs, margs);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsEllipseItem::getPreferredGraphicsItemSize(void) const {
	const GraphicsEllipseItemCfg* cfg = this->getItemConfiguration<GraphicsEllipseItemCfg>();
	return QSizeF(cfg->getRadiusX() * 2., cfg->getRadiusY() * 2.);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsEllipseItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsEllipseItemCfg* cfg = this->getItemConfiguration<GraphicsEllipseItemCfg>();

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
	_painter->setBrush(QtFactory::toQBrush(cfg->getBackgroundPainter()));
	
	_painter->drawEllipse(_rect.center(), _rect.width() / 2., _rect.height() / 2.);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsEllipseItem::setRadius(double _x, double _y) {
	this->getItemConfiguration<GraphicsEllipseItemCfg>()->setRadiusX(_x);
	this->getItemConfiguration<GraphicsEllipseItemCfg>()->setRadiusY(_y);

	this->updateItemGeometry();
	this->raiseEvent(GraphicsItem::ItemResized);
}

void ot::GraphicsEllipseItem::setRadiusX(double _x) {
	this->getItemConfiguration<GraphicsEllipseItemCfg>()->setRadiusX(_x);

	this->updateItemGeometry();
	this->raiseEvent(GraphicsItem::ItemResized);
}

void ot::GraphicsEllipseItem::setRadiusY(double _y) {
	this->getItemConfiguration<GraphicsEllipseItemCfg>()->setRadiusY(_y);

	this->updateItemGeometry();
	this->raiseEvent(GraphicsItem::ItemResized);
}

double ot::GraphicsEllipseItem::getRadiusX(void) const {
	return this->getItemConfiguration<GraphicsEllipseItemCfg>()->getRadiusX();
}

double ot::GraphicsEllipseItem::getRadiusY(void) const {
	return this->getItemConfiguration<GraphicsEllipseItemCfg>()->getRadiusY();
}

void ot::GraphicsEllipseItem::setBackgroundPainter(ot::Painter2D* _painter) {
	this->getItemConfiguration<GraphicsEllipseItemCfg>()->setBackgroundPainer(_painter);
	this->update();
}

const ot::Painter2D* ot::GraphicsEllipseItem::getBackgroundPainter(void) const {
	return this->getItemConfiguration<GraphicsEllipseItemCfg>()->getBackgroundPainter();
}

void ot::GraphicsEllipseItem::setOutline(const PenFCfg& _outline) {
	this->getItemConfiguration<GraphicsEllipseItemCfg>()->setOutline(_outline);
	this->update();
}

const ot::PenFCfg& ot::GraphicsEllipseItem::getOutline(void) const {
	return this->getItemConfiguration<GraphicsEllipseItemCfg>()->getOutline();
}