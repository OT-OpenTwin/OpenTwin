// @otlicense
// File: GraphicsLineItem.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsLineItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsLineItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsLineItem> lineItemRegistrar(ot::GraphicsLineItemCfg::className());

ot::GraphicsLineItem::GraphicsLineItem()
	: ot::CustomGraphicsItem(new GraphicsLineItemCfg)
{

}

ot::GraphicsLineItem::~GraphicsLineItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsLineItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);

	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->setBlockConfigurationNotifications(true);
	this->updateItemGeometry();
	this->setBlockConfigurationNotifications(false);

	return true;
}

QMarginsF ot::GraphicsLineItem::getOutlineMargins(void) const {
	const GraphicsLineItemCfg* cfg = this->getItemConfiguration<GraphicsLineItemCfg>();
	OTAssertNullptr(cfg);
	double margs = cfg->getLineStyle().getWidth() / 2.;
	return QMarginsF(margs, margs, margs, margs);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsLineItem::getPreferredGraphicsItemSize(void) const {
	QLineF l = this->getLine();
	return QSizeF(
		std::max(l.p1().x(), l.p2().x()) - std::min(l.p1().x(), l.p2().x()),
		std::max(l.p1().y(), l.p2().y()) - std::min(l.p1().y(), l.p2().y())
	);
}

void ot::GraphicsLineItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsLineItemCfg* cfg = this->getItemConfiguration<GraphicsLineItemCfg>();
	
	QPen linePen = QtFactory::toQPen(cfg->getLineStyle());

	if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState) {
		if ((this->getGraphicsElementState() & GraphicsElement::SelectedState) && !(this->getGraphicsElementState() & GraphicsElement::HoverState)) {
			Painter2D* newPainter = GraphicsItem::createSelectionBorderPainter();
			linePen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
		else if (this->getGraphicsElementState() & GraphicsElement::HoverState) {
			Painter2D* newPainter = GraphicsItem::createHoverBorderPainter();
			linePen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
	}
	_painter->setPen(linePen);
	_painter->drawLine(this->getLine());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsLineItem::setLine(const Point2DD& _from, const Point2DD& _to) {
	this->prepareGeometryChange();

	this->getItemConfiguration<GraphicsLineItemCfg>()->setFrom(_from);
	this->getItemConfiguration<GraphicsLineItemCfg>()->setTo(_to);
	
	this->setGeometry(QRectF(this->pos(), this->getPreferredGraphicsItemSize()));
	this->raiseEvent(GraphicsItem::ItemResized);
}

void ot::GraphicsLineItem::setLine(const QPointF& _from, const QPointF& _to) {
	this->setLine(QtFactory::toPoint2D(_from), QtFactory::toPoint2D(_to));
}

void ot::GraphicsLineItem::setLine(const QLineF& _line)
{
	this->setLine(QtFactory::toPoint2D(_line.p1()), QtFactory::toPoint2D(_line.p2()));
}

const ot::Point2DD& ot::GraphicsLineItem::getFrom(void) const {
	const GraphicsLineItemCfg* cfg = this->getItemConfiguration<GraphicsLineItemCfg>();
	return cfg->getFrom();
}

const ot::Point2DD& ot::GraphicsLineItem::getTo(void) const {
	const GraphicsLineItemCfg* cfg = this->getItemConfiguration<GraphicsLineItemCfg>();
	return cfg->getTo();
}

QLineF ot::GraphicsLineItem::getLine(void) const {
	const GraphicsLineItemCfg* cfg = this->getItemConfiguration<GraphicsLineItemCfg>();
	return QLineF(QtFactory::toQPoint(cfg->getFrom()), QtFactory::toQPoint(cfg->getTo()));
}

void ot::GraphicsLineItem::setLineStyle(const PenFCfg& _style) {	
	this->getItemConfiguration<GraphicsLineItemCfg>()->setLineStyle(_style);
	this->update();
}

const ot::PenFCfg& ot::GraphicsLineItem::getLineStyle(void) const {
	const GraphicsLineItemCfg* cfg = this->getItemConfiguration<GraphicsLineItemCfg>();
	return cfg->getLineStyle();
}