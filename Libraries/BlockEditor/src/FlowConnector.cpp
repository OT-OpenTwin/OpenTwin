// OpenTwin header
#include <openTwin/FlowConnector.h>
#include <openTwin/FlowConnection.h>
#include <openTwin/FlowBlock.h>

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qfontmetrics.h>

#define FLOW_CONNECTOR_FONT(___name) QFont ___name; ___name.setFamily("Serif"); ___name.setPixelSize(defaultHeight() - 4)

ot::FlowConnector::FlowConnector(const QString& _text, FlowConnectorDirection _direction, FlowConnectorFigure _figure)
	: m_block(nullptr), m_direction(_direction), m_figure(_figure), m_text(_text)
{

}

ot::FlowConnector::~FlowConnector() {
	
}

void ot::FlowConnector::paint(QPainter* _painter, qreal _xLimit, qreal _y) {
	// Figure
	_painter->setBrush(QBrush(m_fillColor));
	_painter->setPen(QPen(m_borderColor));

	if (m_direction == InputFlow) {
		drawFigure(_painter, QPointF(_xLimit, _y));
	}
	else {
		drawFigure(_painter, QPointF(_xLimit - defaultHeight(), _y));
	}

	// Text
	if (!m_text.isEmpty()) {
		FLOW_CONNECTOR_FONT(textFont);

		QFontMetrics fontMetrics(textFont);
		qreal textWidth = fontMetrics.width(m_text);

		_painter->setPen(QPen(QColor(255, 255, 255)));
		_painter->setFont(textFont);

		if (m_direction == InputFlow) {
			_painter->drawText(
				QRectF(
					QPointF(_xLimit + defaultHeight() + 5., _y),
					QPointF(_xLimit + defaultHeight() + 6. + textWidth, _y + defaultHeight())),
				m_text
			);
		}
		else {
			_painter->drawText(
				QRectF(
					QPointF(_xLimit - defaultHeight() - 6. - textWidth, _y),
					QPointF(_xLimit - defaultHeight() - 5., _y + defaultHeight())),
				m_text
			);
		}
	}
}

// #########################################################################################################################

// Setter / Getter

qreal ot::FlowConnector::width(void) const {
	qreal w = defaultHeight();

	if (!m_text.isEmpty()) {
		FLOW_CONNECTOR_FONT(textFont);

		QFontMetrics fontMetrics(textFont);
		w += fontMetrics.width(m_text);
	}

	return w;
}

// #########################################################################################################################

void ot::FlowConnector::attachToBlock(FlowBlock* _block) {
	_block->addConnector(this);
}

void ot::FlowConnector::detach(void) {
	if (m_block) {
		m_block->removeConnector(this);
	}
}

ot::FlowConnection* ot::FlowConnector::connectTo(FlowConnector* _connector, bool applyFillColor) {
	FlowConnection * connection = new ot::FlowConnection(this, _connector);
	addConnection(connection);
	_connector->addConnection(connection);

	if (applyFillColor) connection->setLineColor(m_fillColor);

	return connection;
}

void ot::FlowConnector::addConnection(FlowConnection* _connection) {
	m_connections.append(_connection);
}

void ot::FlowConnector::removeConnection(FlowConnection* _connection) {
	m_connections.removeAll(_connection);
}

// #########################################################################################################################

// Private: Helper

void ot::FlowConnector::drawFigure(QPainter* _painter, const QPointF& _topLeft) {
	// Calculate figure center
	qreal halfHeight = defaultHeight() / 2.;
	m_figureCenter = QPointF(_topLeft.x() + halfHeight, _topLeft.y() + halfHeight);
	if (m_direction == InputFlow) {
		m_connectionPoint = QPointF(_topLeft.x(), _topLeft.y() + halfHeight);
	}
	else if (m_direction == OutputFlow) {
		m_connectionPoint = QPointF(_topLeft.x() + defaultHeight(), _topLeft.y() + halfHeight);
	}
	

	switch (m_figure)
	{
	case ot::Circle:
	{
		_painter->drawEllipse(m_figureCenter, defaultHeight() / 2. - 4., defaultHeight() / 2. - 4.);
	}
		break;
	case ot::Triangle:
	{
		QPainterPath triangle(QPointF(_topLeft.x() + 2., _topLeft.y() + 2.));
		triangle.lineTo(QPointF(_topLeft.x() + defaultHeight() - 2, _topLeft.y() + (defaultHeight() / 2.)));
		triangle.lineTo(QPointF(_topLeft.x() + 2, _topLeft.y() + defaultHeight() - 2));

		_painter->fillPath(triangle, QBrush(m_fillColor));
	}
		break;
	default:
		//todo: replace with otAssert
		assert(0);
		break;
	}

	if (m_block) {
		m_figureCenter = m_block->mapToScene(m_figureCenter);
		m_connectionPoint = m_block->mapToScene(m_connectionPoint);
	}
}