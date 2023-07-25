//! @file BlockConnector.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockConnector.h"

// Qt header
#include <QtGui/qfontmetrics.h>

ot::BlockConnector::BlockConnector()
	: BlockPaintJob(NoDelete), m_titleOrientation(ot::OrientCenter), m_style(ot::ConnectorCircle),
	m_fillColor(128, 128, 128), m_borderColor(0, 0, 0), m_connectorSize(20),
	m_titleFont("Arial"), m_titleMargins(5, 5, 5, 5)
{
	m_titleFont.setPixelSize(12);
}

ot::BlockConnector::~BlockConnector() {

}

QRectF ot::BlockConnector::boundingRect(void) const {
	return QRectF();
}

ot::QueueObject::QueueResultFlags ot::BlockConnector::runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) {


	return QueueObject::JobFailed;
}

QSizeF ot::BlockConnector::calculateSize(void) const {
	QSizeF s(m_connectorSize, m_connectorSize);

	// Get text dimensions
	QFontMetrics m(m_titleFont);
	int tw = m.width(m_title) + m_titleMargins.left() + m_titleMargins.right();
	int th = m.height() + m_titleMargins.top() + m_titleMargins.bottom();

	// Add or adjust the size according to the title orientation
	switch (m_titleOrientation)
	{
	case ot::OrientCenter:
		if (tw > s.width()) { s.setWidth(tw); }
		if (th > s.height()) { s.setHeight(th); }
		break;

	case ot::OrientTop:
	case ot::OrientBottom:
		if (tw > s.width()) { s.setWidth(tw); }
		s.setHeight(s.height() + th);
		break;

	case ot::OrientLeft:
	case ot::OrientRight:
		s.setWidth(s.width() + tw);
		if (th > s.height()) { s.setHeight(th); }
		break;

	case ot::OrientTopRight:
	case ot::OrientTopLeft:
	case ot::OrientBottomRight:
	case ot::OrientBottomLeft:
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
