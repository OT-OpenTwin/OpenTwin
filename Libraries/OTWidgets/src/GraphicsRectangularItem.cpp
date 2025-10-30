// @otlicense
// File: GraphicsRectangularItem.cpp
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
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsRectangularItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsRectangularItem> rectItemRegistrar(ot::GraphicsRectangularItemCfg::className());

ot::GraphicsRectangularItem::GraphicsRectangularItem() 
	: ot::CustomGraphicsItem(new GraphicsRectangularItemCfg)
{

}

ot::GraphicsRectangularItem::~GraphicsRectangularItem() {
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsRectangularItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);

	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->setBlockConfigurationNotifications(true);
	this->updateItemGeometry();
	this->setBlockConfigurationNotifications(false);

	return true;
}

QMarginsF ot::GraphicsRectangularItem::getOutlineMargins(void) const {
	const GraphicsRectangularItemCfg* cfg = this->getItemConfiguration<GraphicsRectangularItemCfg>();
	OTAssertNullptr(cfg);
	double margs = cfg->getOutline().getWidth() / 2.;
	return QMarginsF(margs, margs, margs, margs);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsRectangularItem::getPreferredGraphicsItemSize(void) const {
	return QtFactory::toQSize(this->getItemConfiguration<GraphicsRectangularItemCfg>()->getSize());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsRectangularItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsRectangularItemCfg* cfg = this->getItemConfiguration<GraphicsRectangularItemCfg>();

	QPainterPath pth;
	pth.addRoundedRect(_rect, cfg->getCornerRadius(), cfg->getCornerRadius());

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
	_painter->fillPath(pth, QtFactory::toQBrush(cfg->getBackgroundPainter()));
	_painter->drawPath(pth);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsRectangularItem::setRectangleSize(const Size2DD& _size) {
	GraphicsRectangularItemCfg* cfg = this->getItemConfiguration<GraphicsRectangularItemCfg>();
	cfg->setSize(_size);

	this->updateItemGeometry();
	this->raiseEvent(GraphicsItem::ItemResized);
}

void ot::GraphicsRectangularItem::setRectangleSize(const QSizeF& _size) {
	this->setRectangleSize(QtFactory::toSize2D(_size));	
}

const ot::Size2DD& ot::GraphicsRectangularItem::getRectangleSize(void) const {
	return this->getItemConfiguration<GraphicsRectangularItemCfg>()->getSize();
}

void ot::GraphicsRectangularItem::setBackgroundPainter(ot::Painter2D* _painter) {
	this->getItemConfiguration<GraphicsRectangularItemCfg>()->setBackgroundPainer(_painter);
	this->update();
}

const ot::Painter2D* ot::GraphicsRectangularItem::getBackgroundPainter(void) const {
	return this->getItemConfiguration<GraphicsRectangularItemCfg>()->getBackgroundPainter();
}

void ot::GraphicsRectangularItem::setOutline(const PenFCfg& _outline) {
	this->getItemConfiguration<GraphicsRectangularItemCfg>()->setOutline(_outline);
	this->update();
}

const ot::PenFCfg& ot::GraphicsRectangularItem::getOutline(void) const {
	return this->getItemConfiguration<GraphicsRectangularItemCfg>()->getOutline();
}

void ot::GraphicsRectangularItem::setCornerRadius(int _r) {
	this->getItemConfiguration<GraphicsRectangularItemCfg>()->setCornerRadius(_r);
	this->update();
}

int ot::GraphicsRectangularItem::getCornerRadius(void) const {
	return this->getItemConfiguration<GraphicsRectangularItemCfg>()->getCornerRadius();
}