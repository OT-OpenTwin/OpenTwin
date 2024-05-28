//! @file GraphicsLineItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsLineItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsLineItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsLineItem> rectItemRegistrar(OT_FactoryKey_GraphicsLineItem);

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
	const GraphicsLineItemCfg* cfg = dynamic_cast<const GraphicsLineItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_pen = QtFactory::toPen(cfg->getLineStyle());
	m_line = QLineF(QtFactory::toPoint(cfg->getFrom()), QtFactory::toPoint(cfg->getTo()));

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsLineItem::getPreferredGraphicsItemSize(void) const {
	return QSizeF(
		std::max(m_line.p1().x(), m_line.p2().x()) - std::min(m_line.p1().x(), m_line.p2().x()),
		std::max(m_line.p1().y(), m_line.p2().y()) - std::min(m_line.p1().y(), m_line.p2().y())
	);
}

void ot::GraphicsLineItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	_painter->setPen(m_pen);
	_painter->drawLine(m_line);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsLineItem::setLine(const QLineF& _line)
{
	this->prepareGeometryChange();
	m_line = _line;
	this->setGeometry(QRectF(this->pos(), this->getPreferredGraphicsItemSize()));
	this->raiseEvent(GraphicsItem::ItemResized);
}