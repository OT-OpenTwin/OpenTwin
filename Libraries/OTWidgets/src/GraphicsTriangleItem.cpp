// @otlicense
// File: GraphicsTriangleItem.cpp
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
#include "OTWidgets/GraphicsTriangleItem.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainterpath.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsTriangleItem> triaItemRegistrar(ot::GraphicsTriangleItemCfg::className());

ot::GraphicsTriangleItem::GraphicsTriangleItem()
	: ot::CustomGraphicsItem(new GraphicsTriangleItemCfg)
{

}

ot::GraphicsTriangleItem::~GraphicsTriangleItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsTriangleItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);

	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->setBlockConfigurationNotifications(true);
	this->updateItemGeometry();
	this->setBlockConfigurationNotifications(false);

	return true;
}

QMarginsF ot::GraphicsTriangleItem::getOutlineMargins(void) const {
	const GraphicsTriangleItemCfg* cfg = this->getItemConfiguration<GraphicsTriangleItemCfg>();
	OTAssertNullptr(cfg);
	double margs = cfg->getOutline().getWidth() / 2.;
	return QMarginsF(margs, margs, margs, margs);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsTriangleItem::getPreferredGraphicsItemSize(void) const {
	return QtFactory::toQSize(this->getItemConfiguration<GraphicsTriangleItemCfg>()->getSize());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsTriangleItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {	
	const GraphicsTriangleItemCfg* cfg = this->getItemConfiguration<GraphicsTriangleItemCfg>();

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

	_painter->setBrush(QtFactory::toQBrush(cfg->getBackgroundPainter()));
	_painter->setPen(borderPen);

	switch (cfg->getTriangleShape())
	{
	case ot::GraphicsTriangleItemCfg::Triangle: 
		this->paintTriangle(_painter, _opt, _widget, _rect, cfg);
		break;
	case ot::GraphicsTriangleItemCfg::Kite:
		this->paintKite(_painter, _opt, _widget, _rect, cfg);
		break;
	case ot::GraphicsTriangleItemCfg::IceCone:
		this->paintIceCone(_painter, _opt, _widget, _rect, cfg);
		break;
	default:
		OT_LOG_EAS("Unknown triangle shape (" + std::to_string((int)cfg->getTriangleShape()) + ")");
		break;
	}
}

void ot::GraphicsTriangleItem::paintTriangle(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect, const GraphicsTriangleItemCfg* _triangleConfig) {
	// Set path depending on the direction
	QPainterPath path;

	switch (_triangleConfig->getTriangleDirection())
	{
	case ot::GraphicsTriangleItemCfg::Left:
		path.moveTo(QPointF(_rect.topLeft().x(), _rect.center().y()));
		path.lineTo(_rect.topRight());
		path.lineTo(_rect.bottomRight());
		path.lineTo(QPointF(_rect.topLeft().x(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Up:
		path.moveTo(QPointF(_rect.center().x(), _rect.topLeft().y()));
		path.lineTo(_rect.bottomRight());
		path.lineTo(_rect.bottomLeft());
		path.lineTo(QPointF(_rect.center().x(), _rect.topLeft().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Right:
		path.moveTo(QPointF(_rect.bottomRight().x(), _rect.center().y()));
		path.lineTo(_rect.bottomLeft());
		path.lineTo(_rect.topLeft());
		path.lineTo(QPointF(_rect.bottomRight().x(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Down:
		path.moveTo(QPointF(_rect.center().x(), _rect.bottomRight().y()));
		path.lineTo(_rect.topLeft());
		path.lineTo(_rect.topRight());
		path.lineTo(QPointF(_rect.center().x(), _rect.bottomRight().y()));
		break;
	default:
		OT_LOG_EAS("Unknown triangle direction (" + std::to_string(_triangleConfig->getTriangleDirection()) + ")");
		break;
	}

	_painter->fillPath(path, _painter->brush());
	_painter->drawPath(path);
}

void ot::GraphicsTriangleItem::paintKite(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect, const GraphicsTriangleItemCfg* _triangleConfig) {
	// Set path depending on the direction
	QPainterPath path;

	switch (_triangleConfig->getTriangleDirection())
	{
	case ot::GraphicsTriangleItemCfg::Left:
		path.moveTo(QPointF(_rect.left(), _rect.center().y()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 3.f, _rect.top() + _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.right(), _rect.center().y()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 3.f, _rect.bottom() - _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.left(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Up:
		path.moveTo(QPointF(_rect.center().x(), _rect.top()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 6.f, _rect.bottom() - _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.bottom()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 6.f, _rect.bottom() - _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.top()));
		break;
	case ot::GraphicsTriangleItemCfg::Right:
		path.moveTo(QPointF(_rect.right(), _rect.center().y()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 3.f, _rect.bottom() - _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.left(), _rect.center().y()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 3.f, _rect.top() + _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.right(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Down:
		path.moveTo(QPointF(_rect.center().x(), _rect.bottom()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 6.f, _rect.top() + _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.top()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 6.f, _rect.top() + _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.bottom()));
		break;
	default:
		OT_LOG_EAS("Unknown triangle direction (" + std::to_string(_triangleConfig->getTriangleDirection()) + ")");
		break;
	}

	_painter->fillPath(path, _painter->brush());
	_painter->drawPath(path);
}

void ot::GraphicsTriangleItem::paintIceCone(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect, const GraphicsTriangleItemCfg* _triangleConfig) {
	// Set path depending on the direction
	QPainterPath path;

	switch (_triangleConfig->getTriangleDirection())
	{
	case ot::GraphicsTriangleItemCfg::Left:
		path.moveTo(QPointF(_rect.left(), _rect.center().y()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 3.f, _rect.top() + _rect.height() / 6.f));
		path.cubicTo(QPointF(_rect.right(), _rect.center().y() - _rect.height() / 6.f), QPointF(_rect.right(), _rect.center().y() + _rect.height() / 6.f), QPointF(_rect.right() - _rect.width() / 3.f, _rect.bottom() - _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.left(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Up:
		path.moveTo(QPointF(_rect.center().x(), _rect.top()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 6.f, _rect.bottom() - _rect.height() / 3.f));
		path.cubicTo(QPointF(_rect.center().x() + _rect.width() / 6.f, _rect.bottom()), QPointF(_rect.center().x() - _rect.width() / 6.f, _rect.bottom()), QPointF(_rect.left() + _rect.width() / 6.f, _rect.bottom() - _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.top()));
		break;
	case ot::GraphicsTriangleItemCfg::Right:
		path.moveTo(QPointF(_rect.right(), _rect.center().y()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 3.f, _rect.bottom() - _rect.height() / 6.f));
		path.cubicTo(QPointF(_rect.left(), _rect.center().y() + _rect.height() / 6.f), QPointF(_rect.left(), _rect.center().y() - _rect.height() / 6.f), QPointF(_rect.left() + _rect.width() / 3.f, _rect.top() + _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.right(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Down:
		path.moveTo(QPointF(_rect.center().x(), _rect.bottom()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 6.f, _rect.top() + _rect.height() / 3.f));
		path.cubicTo(QPointF(_rect.center().x() - _rect.width() / 6.f, _rect.top()), QPointF(_rect.center().x() + _rect.width() / 6.f, _rect.top()), QPointF(_rect.right() - _rect.width() / 6.f, _rect.top() + _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.bottom()));
		break;
	default:
		OT_LOG_EAS("Unknown triangle direction (" + std::to_string(_triangleConfig->getTriangleDirection()) + ")");
		break;
	}

	_painter->fillPath(path, _painter->brush());
	_painter->drawPath(path);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsTriangleItem::setTriangleSize(const Size2DD& _size) {
	GraphicsTriangleItemCfg* cfg = this->getItemConfiguration<GraphicsTriangleItemCfg>();
	cfg->setSize(_size);

	this->updateItemGeometry();
	this->raiseEvent(GraphicsItem::ItemResized);
}

void ot::GraphicsTriangleItem::setTriangleSize(const QSizeF& _size) {
	this->setTriangleSize(QtFactory::toSize2D(_size));
}

const ot::Size2DD& ot::GraphicsTriangleItem::getTriangleSize(void) const {
	return this->getItemConfiguration<GraphicsTriangleItemCfg>()->getSize();
}

void ot::GraphicsTriangleItem::setBackgroundPainter(ot::Painter2D* _painter) {
	this->getItemConfiguration<GraphicsTriangleItemCfg>()->setBackgroundPainer(_painter);
	this->update();
}

const ot::Painter2D* ot::GraphicsTriangleItem::getBackgroundPainter(void) const {
	return this->getItemConfiguration<GraphicsTriangleItemCfg>()->getBackgroundPainter();
}

void ot::GraphicsTriangleItem::setOutline(const PenFCfg& _outline) {
	this->getItemConfiguration<GraphicsTriangleItemCfg>()->setOutline(_outline);
	this->update();
}

const ot::PenFCfg& ot::GraphicsTriangleItem::getOutline(void) const {
	return this->getItemConfiguration<GraphicsTriangleItemCfg>()->getOutline();
}

void ot::GraphicsTriangleItem::setTriangleShape(ot::GraphicsTriangleItemCfg::TriangleShape _shape) {
	this->getItemConfiguration<GraphicsTriangleItemCfg>()->setTriangleShape(_shape);
	this->update();
}

ot::GraphicsTriangleItemCfg::TriangleShape ot::GraphicsTriangleItem::getTriangleShape(void) const {
	return this->getItemConfiguration<GraphicsTriangleItemCfg>()->getTriangleShape();
}

void ot::GraphicsTriangleItem::setTriangleDirection(ot::GraphicsTriangleItemCfg::TriangleDirection _direction) {
	this->getItemConfiguration<GraphicsTriangleItemCfg>()->setTriangleDirection(_direction);
	this->update();
}

ot::GraphicsTriangleItemCfg::TriangleDirection ot::GraphicsTriangleItem::getTrianlgeDirection(void) const {
	return this->getItemConfiguration<GraphicsTriangleItemCfg>()->getTriangleDirection();
}