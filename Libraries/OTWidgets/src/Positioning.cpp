//! \file Positioning.cpp
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Positioning.h"

QRect ot::calculateChildRect(const QRect& _parentRect, const QSize& _childSize, ot::Alignment _childAlignment) {
	// Get the top left point of the outer rectangle
	QPoint pt(_parentRect.topLeft());

	// Align inner rectangle
	switch (_childAlignment)
	{
	case ot::AlignCenter:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case ot::AlignTop:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		break;
	case ot::AlignTopRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		break;
	case ot::AlignRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case ot::AlignBottomRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignBottom:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignBottomLeft:
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignLeft:
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case ot::AlignTopLeft:
		break;
	default:
		OT_LOG_EA("Unknown Alignment");
		break;
	}

	return QRect(pt, _childSize);
}

QRectF ot::calculateChildRect(const QRectF& _parentRect, const QSizeF& _childSize, ot::Alignment _childAlignment) {
	// Get the top left point of the outer rectangle
	QPointF pt(_parentRect.topLeft());

	// Align inner rectangle
	switch (_childAlignment)
	{
	case ot::AlignCenter:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case ot::AlignTop:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		break;
	case ot::AlignTopRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		break;
	case ot::AlignRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case ot::AlignBottomRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignBottom:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignBottomLeft:
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignLeft:
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case ot::AlignTopLeft:
		break;
	default:
		OT_LOG_EA("Unknown Alignment");
		break;
	}

	return QRectF(pt, _childSize);
}