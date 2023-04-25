#include <openTwin/FlowBlock.h>
#include <openTwin/FlowConnector.h>

#include <QtGui/qpainter.h>
#include <QtGui/qbrush.h>
#include <QtGui/qfontmetrics.h>
#include <QtWidgets/qstyleoption.h>
#include <QtWidgets/qwidget.h>

#define FLOW_TITLE_HEIGHT 40.

#define FLOW_CONNECTOR_PADDING 8.

#define FLOW_TITLE_FONT(___name) QFont ___name; ___name.setFamily("Serif"); ___name.setPixelSize((int)FLOW_TITLE_HEIGHT - 4)

ot::FlowBlock::FlowBlock(const QString& _title) : m_title(_title), m_titleColor(0, 255, 0), 
m_titleImage(nullptr), m_bottomLeftImage(nullptr), m_bottomRightImage(nullptr) 
{
	setFlags(flags() | QGraphicsItem::ItemIsMovable);
}

ot::FlowBlock::~FlowBlock() {
	for (auto c : m_inputConnectors) delete c;
	for (auto c : m_outputConnectors) delete c;
	if (m_titleImage) delete m_titleImage;
	if (m_bottomLeftImage) delete m_bottomLeftImage;
	if (m_bottomRightImage) delete m_bottomRightImage;
}

qreal ot::FlowBlock::blockWidth(void) const {
	qreal titleWidth = 20;
	if (m_titleImage) {
		titleWidth += m_titleImage->width();
	}
	
	FLOW_TITLE_FONT(titleFont);
	QFontMetrics fontMetrics(titleFont);
	titleWidth += fontMetrics.width(m_title);

	qreal maxIn = 0.;
	for (auto c : m_inputConnectors) {
		qreal w = c->width();
		if (w > maxIn) {
			maxIn = w;
		}
	}

	qreal maxOut = 0.;
	for (auto c : m_outputConnectors) {
		qreal w = c->width();
		if (w > maxOut) {
			maxOut = w;
		}
	}
	maxIn += maxOut + 10;

	if (titleWidth > maxIn) {
		return titleWidth;
	}
	else {
		return maxIn;
	}
}

qreal ot::FlowBlock::blockHeigth(void) const {
	qreal ret = 2 * FLOW_TITLE_HEIGHT;
	if (m_inputConnectors.count() > m_outputConnectors.count()) {
		ret += m_inputConnectors.count() * (FlowConnector::defaultHeight() + FLOW_CONNECTOR_PADDING);
	}
	else {
		ret += m_outputConnectors.count() * (FlowConnector::defaultHeight() + FLOW_CONNECTOR_PADDING);
	}

	return ret;
}

ot::BlockConnector* ot::FlowBlock::connectorAt(const QPointF& _point) const {
	QPointF pt = mapToItem(this, _point);
	qreal w = blockWidth();

	// Expected index
	int i = (int)(pt.y() - FLOW_TITLE_HEIGHT) / (FlowConnector::defaultHeight() + FLOW_CONNECTOR_PADDING);

	if (pt.x() >= 5 && pt.x() < FlowConnector::defaultHeight() + 5) {
		// Input
		if (i >= 0 && i < m_inputConnectors.count()) {
			return m_inputConnectors.at(i);
		}
	}
	else if (pt.x() >= (w - 5 - FlowConnector::defaultHeight()) && pt.x() < w - 5) {
		// Output
		if (i >= 0 && i < m_outputConnectors.count()) {
			return m_outputConnectors.at(i);
		}
	}
	// No match
	return nullptr;
}

void ot::FlowBlock::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	QPen defaultBorderPen(QColor(60, 60, 60));
	defaultBorderPen.setWidth(1);

	// Background
	_painter->setBrush(QBrush(QColor(5, 5, 5)));
	_painter->setPen(defaultBorderPen);
	_painter->drawRoundedRect(boundingRect(), 5, 5);

	QPointF ptTopLeft = boundingRect().topLeft();

	qreal currentY = ptTopLeft.y() + FLOW_TITLE_HEIGHT;
	qreal rightX = boundingRect().topRight().x();
	qreal bottomY = boundingRect().bottomLeft().y();

	qreal titleX = ptTopLeft.x() + 5;

	QLinearGradient titleGradient(ptTopLeft, QPointF(rightX, currentY));
	titleGradient.setColorAt(0.0, m_titleColor);
	titleGradient.setColorAt(1.0, QColor(200, 200, 200));

	FLOW_TITLE_FONT(titleFont);
	
	// Title background
	_painter->setBrush(titleGradient);
	_painter->setPen(defaultBorderPen);
	_painter->drawRoundedRect(QRectF(ptTopLeft, QPointF(rightX, currentY)), 5, 5);

	// Title Image
	if (m_titleImage) {
		_painter->drawPixmap(QPointF(titleX, ptTopLeft.y()),*m_titleImage);
		titleX += (FLOW_TITLE_HEIGHT);
	}

	// Title text
	QPen defaultTextPen(QColor(0, 0, 0));
	defaultTextPen.setWidth(1);

	_painter->setFont(titleFont);
	_painter->setPen(defaultTextPen);
	_painter->drawText(QPointF(titleX, currentY - 10), m_title);


	currentY += 5.;
	qreal startInputY = currentY;
	qreal endInputY = currentY;
	qreal leftInputLimit = ptTopLeft.x() + 5;
	qreal rightInputLimit = rightX - 5;

	// Inputs
	int ct = 0;

	for (FlowConnector * connector : m_inputConnectors) {
		connector->paint(_painter, leftInputLimit, currentY);
		currentY += FlowConnector::defaultHeight() + FLOW_CONNECTOR_PADDING;
	}
	
	endInputY = currentY;
	currentY = startInputY;
	
	for (FlowConnector* connector : m_outputConnectors) {
		connector->paint(_painter, rightInputLimit, currentY);
		currentY += FlowConnector::defaultHeight() + FLOW_CONNECTOR_PADDING;
	}

	if (currentY > endInputY) {
		endInputY = currentY;
	}
}

void ot::FlowBlock::addConnector(FlowConnector* _connector) {
	switch (_connector->direction()) {
	case ot::InputFlow:
		m_inputConnectors.append(_connector);
		break;
	case ot::OutputFlow:
		m_outputConnectors.append(_connector);
		break;
	default:
		//todo: switch to otAssert
		assert(0);
		return;
	}
	_connector->setBlock(this);
}

void ot::FlowBlock::removeConnector(FlowConnector* _connector) {
	_connector->setBlock(nullptr);
	m_inputConnectors.removeAll(_connector);
	m_outputConnectors.removeAll(_connector);
}

void ot::FlowBlock::setTitleImage(const QPixmap& _image) {
	if (m_titleImage) delete m_titleImage;
	m_titleImage = new QPixmap(
		_image.scaled((int)FLOW_TITLE_HEIGHT - 4,
			(int)FLOW_TITLE_HEIGHT - 4,
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation)
	);
}

QPixmap ot::FlowBlock::titleImage(void) const {
	if (m_titleImage) return *m_titleImage;
	else return QPixmap();
}

void ot::FlowBlock::setBottomLeftImage(const QPixmap& _image) {
	if (m_bottomLeftImage) delete m_bottomLeftImage;
	m_bottomLeftImage = new QPixmap(_image);
}

QPixmap ot::FlowBlock::bottomLeftImage(void) const {
	if (m_bottomLeftImage) return *m_bottomLeftImage;
	else return QPixmap();
}

void ot::FlowBlock::setBottomRightImage(const QPixmap& _image) {
	if (m_bottomRightImage) delete m_bottomRightImage;
	m_bottomRightImage = new QPixmap(_image);
}

QPixmap ot::FlowBlock::bottomRightImage(void) const {
	if (m_bottomRightImage) return *m_bottomRightImage;
	else return QPixmap();
}
