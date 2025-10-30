// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsArcItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsArcItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsArcItem> arcItemRegistrar(ot::GraphicsArcItemCfg::className());

ot::GraphicsArcItem::GraphicsArcItem()
	: ot::CustomGraphicsItem(new GraphicsArcItemCfg)
{

}

ot::GraphicsArcItem::~GraphicsArcItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsArcItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);

	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->setBlockConfigurationNotifications(true);
	this->updateItemGeometry();
	this->setBlockConfigurationNotifications(false);

	return true;
}

QMarginsF ot::GraphicsArcItem::getOutlineMargins(void) const {
	const GraphicsArcItemCfg* cfg = this->getItemConfiguration<GraphicsArcItemCfg>();
	OTAssertNullptr(cfg);
	double margs = cfg->getLineStyle().getWidth() / 2.;
	return QMarginsF(margs, margs, margs, margs);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsArcItem::getPreferredGraphicsItemSize(void) const {
	return QtFactory::toQSize(this->getItemConfiguration<GraphicsArcItemCfg>()->getRect().getSize());
}

void ot::GraphicsArcItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsArcItemCfg* cfg = this->getItemConfiguration<GraphicsArcItemCfg>();
	
	QPen borderPen = QtFactory::toQPen(cfg->getLineStyle());

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
	_painter->drawArc(QtFactory::toQRect(this->getArcRect()), this->getStartAngle(), this->getSpanAngle());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsArcItem::setArcRect(const RectD& _rect) {
	this->getItemConfiguration<GraphicsArcItemCfg>()->setRect(_rect);
	this->update();
}

void ot::GraphicsArcItem::setArcRect(const QRectF& _rect) {
	this->setArcRect(QtFactory::toRect(_rect));
}

const ot::RectD& ot::GraphicsArcItem::getArcRect(void) const {
	return this->getItemConfiguration<GraphicsArcItemCfg>()->getRect();
}

void ot::GraphicsArcItem::setStartAngle(double _angle) {
	this->getItemConfiguration<GraphicsArcItemCfg>()->setStartAngle(_angle);
	this->update();
}

double ot::GraphicsArcItem::getStartAngle(void) const {
	return this->getItemConfiguration<GraphicsArcItemCfg>()->getStartAngle();
}

void ot::GraphicsArcItem::setSpanAngle(double _angle) {
	this->getItemConfiguration<GraphicsArcItemCfg>()->setSpanAngle(_angle);
	this->update();
}

double ot::GraphicsArcItem::getSpanAngle(void) const {
	return this->getItemConfiguration<GraphicsArcItemCfg>()->getSpanAngle();
}

void ot::GraphicsArcItem::setLineStyle(const PenFCfg& _style) {
	this->getItemConfiguration<GraphicsArcItemCfg>()->setLineStyle(_style);
	this->update();
}

const ot::PenFCfg& ot::GraphicsArcItem::getLineStyle(void) const {
	const GraphicsArcItemCfg* cfg = this->getItemConfiguration<GraphicsArcItemCfg>();
	return cfg->getLineStyle();
}