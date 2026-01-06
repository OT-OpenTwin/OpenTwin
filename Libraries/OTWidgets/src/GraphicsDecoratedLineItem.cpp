// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsDecoratedLineItem.h"

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsDecoratedLineItem> registrar(ot::GraphicsDecoratedLineItemCfg::className());

ot::GraphicsDecoratedLineItem::GraphicsDecoratedLineItem() : GraphicsLineItem() {}

ot::GraphicsDecoratedLineItem::~GraphicsDecoratedLineItem() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItem

bool ot::GraphicsDecoratedLineItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	const GraphicsDecoratedLineItemCfg* cfg = dynamic_cast<const GraphicsDecoratedLineItemCfg*>(_cfg);
	if (!cfg) {
		OT_LOG_E("Invalid configuration object provided");
		return false;
	}

	if (!GraphicsLineItem::setupFromConfig(cfg)) {
		return false;
	}

	m_fromDecoration.setConfiguration(cfg->getFromDecoration());
	m_toDecoration.setConfiguration(cfg->getToDecoration());

	return true;
}

QMarginsF ot::GraphicsDecoratedLineItem::getOutlineMargins(void) const {
	QMarginsF baseMargins = GraphicsLineItem::getOutlineMargins();

	double decoPenWidth = m_fromDecoration.getConfiguration().getOutlinePen().getWidth();
	decoPenWidth = std::max(decoPenWidth, m_toDecoration.getConfiguration().getOutlinePen().getWidth());

	baseMargins.setLeft(std::max(baseMargins.left(), decoPenWidth));
	baseMargins.setTop(std::max(baseMargins.top(), decoPenWidth));
	baseMargins.setRight(std::max(baseMargins.right(), decoPenWidth));
	baseMargins.setBottom(std::max(baseMargins.bottom(), decoPenWidth));

	return baseMargins;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsDecoratedLineItem::getPreferredGraphicsItemSize(void) const {
	QSizeF baseSize = GraphicsLineItem::getPreferredGraphicsItemSize();

	QPainterPath fromPath = m_fromDecoration.createDecorationPath();
	QPainterPath toPath = m_toDecoration.createDecorationPath();

	QRectF fromRect = fromPath.boundingRect();
	QRectF toRect = toPath.boundingRect();

	baseSize.setWidth(baseSize.width() + (fromRect.width() / 2.) + (toRect.width() / 2.));
	baseSize.setHeight(baseSize.height() + (fromRect.height() / 2.) + (toRect.height() / 2.));

	return baseSize;
}

void ot::GraphicsDecoratedLineItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	GraphicsLineItem::paintCustomItem(_painter, _opt, _widget, _rect);

	QLineF line = this->getLine();

	Size2DD fromSize = m_fromDecoration.getConfiguration().getSize();
	Size2DD toSize = m_toDecoration.getConfiguration().getSize();

	// Direction vector for shifting inside the line
	QPointF dir = line.unitVector().p2() - line.unitVector().p1();

	// Shift inward
	double fromOffset = fromSize.width() * 0.5;
	double toOffset = toSize.width() * 0.5;

	QPointF innerFrom = line.p1() + dir * fromOffset;
	QPointF innerTo = line.p2() - dir * toOffset;

	m_fromDecoration.paintDecoration(
		_painter,
		innerFrom,
		-line.angle()
	);
	m_toDecoration.paintDecoration(
		_painter,
		innerTo,
		180. + (-line.angle())
	);
}

void ot::GraphicsDecoratedLineItem::setFromDecoration(const GraphicsDecorationCfg& _config) {
	prepareGeometryChange();
	m_fromDecoration.setConfiguration(_config);
	update();
}

void ot::GraphicsDecoratedLineItem::setToDecoration(const GraphicsDecorationCfg& _config) {
	prepareGeometryChange();
	m_toDecoration.setConfiguration(_config);
	update();
}
