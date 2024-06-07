//! @file GraphicsTextItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsTextItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qfontmetrics.h>
#include <QtGui/qtextdocument.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsTextItem> textItemRegistrar(OT_FactoryKey_GraphicsTextItem);

ot::GraphicsTextItem::GraphicsTextItem()
	: ot::CustomGraphicsItem(new GraphicsTextItemCfg)
{

}

ot::GraphicsTextItem::~GraphicsTextItem() {

}

bool ot::GraphicsTextItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	
	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->updateItemGeometry();

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsTextItem::getPreferredGraphicsItemSize(void) const {
	const GraphicsTextItemCfg* cfg = this->getItemConfiguration<GraphicsTextItemCfg>();

	QFontMetrics m(QtFactory::toQFont(cfg->getTextFont()));
	return m.size(Qt::TextSingleLine, QString::fromStdString(cfg->getText()));
}

void ot::GraphicsTextItem::finalizeGraphicsItem(void) {
	GraphicsTextItemCfg* cfg = this->getItemConfiguration<GraphicsTextItemCfg>();
	if (cfg->getTextIsReference()) {
		this->prepareGeometryChange();
		GraphicsItem* rootItem = this->getRootItem();
		OTAssertNullptr(rootItem);
		cfg->setText(rootItem->getConfiguration()->getStringForKey(cfg->getText()));
		cfg->setTextIsReference(false);
		this->updateItemGeometry();
	}
}

void ot::GraphicsTextItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsTextItemCfg* cfg = this->getItemConfiguration<GraphicsTextItemCfg>();

	_painter->setFont(QtFactory::toQFont(cfg->getTextFont()));
	_painter->setPen(QtFactory::toQPen(cfg->getTextStyle()));
	_painter->drawText(_rect, QtFactory::toQAlignment(this->getGraphicsItemAlignment()), QString::fromStdString(cfg->getText()));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsTextItem::setText(const std::string& _text) {
	this->prepareGeometryChange();
	this->getItemConfiguration<GraphicsTextItemCfg>()->setText(_text);
	this->updateItemGeometry();
}

const std::string& ot::GraphicsTextItem::getText(void) const {
	return this->getItemConfiguration<GraphicsTextItemCfg>()->getText();
}

void ot::GraphicsTextItem::setFont(const Font& _font) {
	this->prepareGeometryChange();
	this->getItemConfiguration<GraphicsTextItemCfg>()->setTextFont(_font);
	this->updateItemGeometry();
}

const ot::Font& ot::GraphicsTextItem::getFont(void) const {
	return this->getItemConfiguration<GraphicsTextItemCfg>()->getTextFont();
}

void ot::GraphicsTextItem::setTextColor(const Color& _color) {
	this->getItemConfiguration<GraphicsTextItemCfg>()->setTextColor(_color);
	this->update();
}

void ot::GraphicsTextItem::setTextPainter(Painter2D* _painter) {
	this->getItemConfiguration<GraphicsTextItemCfg>()->setTextPainter(_painter);
	this->update();
}

const ot::Painter2D* ot::GraphicsTextItem::getTextPainter(void) const {
	return this->getItemConfiguration<GraphicsTextItemCfg>()->getTextPainter();
}

void ot::GraphicsTextItem::setTextIsReference(bool _isReference) {
	this->getItemConfiguration<GraphicsTextItemCfg>()->setTextIsReference(_isReference);
	this->update();
}

bool ot::GraphicsTextItem::getTextIsReference(void) const {
	return this->getItemConfiguration<GraphicsTextItemCfg>()->getTextIsReference();
}