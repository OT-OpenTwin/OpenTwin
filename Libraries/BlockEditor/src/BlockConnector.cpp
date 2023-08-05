//! @file BlockConnector.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockConnector.h"
#include "OTBlockEditor/BlockPaintJob.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/otAssert.h"

// Qt header
#include <QtGui/qfontmetrics.h>
#include <QtGui/qpen.h>
#include <QtGui/qpainter.h>

ot::BlockConnector::BlockConnector()
	: BlockPaintJob(NoDelete), m_titleAlignment(ot::AlignCenter), m_style(ot::ConnectorCircle),
	m_fillColor(128, 128, 128), m_borderColor(0, 0, 0), m_connectorSize(12),
	m_titleFont("Arial"), m_titleMargins(5, 5, 5, 5)
{
	m_titleFont.setPixelSize(12);
}

ot::BlockConnector::~BlockConnector() {

}

QRectF ot::BlockConnector::boundingRect(void) const {
	QPointF p(scenePos());
	QSizeF s(calculateSize());

	return QRectF(QPointF(p.x() - (s.width() / 2.), p.y() - (s.height() / 2.)), s);
}

ot::QueueObject::QueueResultFlags ot::BlockConnector::runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) {
	BlockPaintJobArg* arg = dynamic_cast<BlockPaintJobArg*>(_arg);
	if (arg == nullptr) {
		OT_LOG_EA("Block paint job arg cast failed");
		return QueueObject::JobFailed;
	}

	// Setup painter
	QPen p;
	p.setColor(m_borderColor);
	p.setWidth(1);
	arg->painter()->setPen(p);
	arg->painter()->setBrush(QBrush(m_fillColor));

	QRectF r(rect());
	OT_LOG_D("Rect on: (" + std::to_string(r.x()) + "; " + std::to_string(r.y()) + ")");

	// Paint symbol
	switch (m_style)
	{
	case ot::ConnectorNone: return QueueObject::Ok;
	case ot::ConnectorImage:
		otAssert(0, "Not implemented yet");
		break;
	case ot::ConnectorPoint:
		arg->painter()->drawPoint(r.center());
		break;
	case ot::ConnectorCircle:
		arg->painter()->drawEllipse(r.center(), m_connectorSize / 2., m_connectorSize / 2.);
		break;
	case ot::ConnectorTriangleIn:
		otAssert(0, "Not implemented yet");
		break;
	case ot::ConnectorTriangleOut:
		otAssert(0, "Not implemented yet");
		break;
	case ot::ConnectorTriangleUp:
		otAssert(0, "Not implemented yet");
		break;
	case ot::ConnectorTriangleRight:
		otAssert(0, "Not implemented yet");
		break;
	case ot::ConnectorTriangleDown:
		otAssert(0, "Not implemented yet");
		break;
	case ot::ConnectorTriangleLeft:
		otAssert(0, "Not implemented yet");
		break;
	default:
		otAssert(0, "Not implemented yet");
		break;
	}

	return QueueObject::Ok;
}

QSizeF ot::BlockConnector::calculateSize(void) const {
	QSizeF s(m_connectorSize, m_connectorSize);

	// Get text dimensions
	QFontMetrics m(m_titleFont);
	int tw = m.width(m_title) + m_titleMargins.left() + m_titleMargins.right();
	int th = m.height() + m_titleMargins.top() + m_titleMargins.bottom();

	// Add or adjust the size according to the title alignment
	switch (m_titleAlignment)
	{
	case ot::AlignCenter:
		if (tw > s.width()) { s.setWidth(tw); }
		if (th > s.height()) { s.setHeight(th); }
		break;

	case ot::AlignTop:
	case ot::AlignBottom:
		if (tw > s.width()) { s.setWidth(tw); }
		s.setHeight(s.height() + th);
		break;

	case ot::AlignLeft:
	case ot::AlignRight:
		s.setWidth(s.width() + tw);
		if (th > s.height()) { s.setHeight(th); }
		break;

	case ot::AlignTopRight:
	case ot::AlignTopLeft:
	case ot::AlignBottomRight:
	case ot::AlignBottomLeft:
		s.setWidth(s.width() + tw);
		s.setHeight(s.height() + th);
		break;
	
	default:
		break;
	}

	// Check max
	if (widthLimit().isMaxSet()) {
		if (s.width() > widthLimit().max()) {
			s.setWidth(widthLimit().max());
		}
	}
	if (heightLimit().isMaxSet()) {
		if (s.height() > heightLimit().max()) {
			s.setHeight(heightLimit().max());
		}
	}

	// Check min
	if (widthLimit().isMinSet()) {
		if (s.width() < widthLimit().min()) {
			s.setWidth(widthLimit().min());
		}
	}
	if (heightLimit().isMinSet()) {
		if (s.height() < heightLimit().min()) {
			s.setHeight(heightLimit().min());
		}
	}

	// Add margins and apply the limits
	return applyLimits(addMargins(s));
}

QRectF ot::BlockConnector::rect(void) const {
	return this->boundingRect();
}
