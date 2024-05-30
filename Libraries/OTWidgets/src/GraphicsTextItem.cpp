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
	const GraphicsTextItemCfg* cfg = dynamic_cast<const GraphicsTextItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_font.setPixelSize(cfg->textFont().size());
	m_font.setItalic(cfg->textFont().isItalic());
	m_font.setBold(cfg->textFont().isBold());

	m_pen.setBrush(QtFactory::toQBrush(cfg->textPainter()));

	m_text = QString::fromStdString(cfg->text());

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsTextItem::getPreferredGraphicsItemSize(void) const {
	QFontMetrics m(m_font);
	return m.size(Qt::TextSingleLine, m_text);
}

void ot::GraphicsTextItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	_painter->setFont(m_font);
	_painter->setPen(m_pen);
	_painter->drawText(_rect, QtFactory::toQAlignment(this->getGraphicsItemAlignment()), m_text);
}
