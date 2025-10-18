//! @file GraphicsTextItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsTextItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qfontmetrics.h>
#include <QtGui/qtextdocument.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsTextItem> textItemRegistrar(ot::GraphicsTextItemCfg::className());

ot::GraphicsTextItem::GraphicsTextItem()
	: ot::CustomGraphicsItem(new GraphicsTextItemCfg)
{

}

ot::GraphicsTextItem::~GraphicsTextItem() {

}

bool ot::GraphicsTextItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	
	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->setBlockConfigurationNotifications(true);
	this->updateItemGeometry();
	this->setBlockConfigurationNotifications(false);

	return true;
}

QMarginsF ot::GraphicsTextItem::getOutlineMargins(void) const {
	const GraphicsTextItemCfg* cfg = this->getItemConfiguration<GraphicsTextItemCfg>();
	OTAssertNullptr(cfg);
	double margs = cfg->getTextStyle().getWidth() / 2.;
	return QMarginsF(margs, margs, margs, margs);
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

		this->setBlockConfigurationNotifications(true);
		this->updateItemGeometry();
		this->setBlockConfigurationNotifications(false);
	}
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

void ot::GraphicsTextItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsTextItemCfg* cfg = this->getItemConfiguration<GraphicsTextItemCfg>();

	QPen textPen = QtFactory::toQPen(cfg->getTextStyle());

	if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState) {
		if ((this->getGraphicsElementState() & GraphicsElement::SelectedState) && !(this->getGraphicsElementState() & GraphicsElement::HoverState)) {
			Painter2D* newPainter = GraphicsItem::createSelectionBorderPainter();
			textPen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
		else if (this->getGraphicsElementState() & GraphicsElement::HoverState) {
			Painter2D* newPainter = GraphicsItem::createHoverBorderPainter();
			textPen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
	}

	_painter->setFont(QtFactory::toQFont(cfg->getTextFont()));
	_painter->setPen(textPen);
	
	_painter->drawText(_rect, QString::fromStdString(cfg->getText()), QTextOption(QtFactory::toQAlignment(this->getGraphicsItemAlignment())));
}
