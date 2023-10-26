//! @file OTQtConverter.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/Logger.h"
#include "OTWidgets/OTQtConverter.h"

QColor ot::OTQtConverter::toQt(const ot::Color& _color) {
	return QColor(_color.rInt(), _color.gInt(), _color.bInt(), _color.aInt());
}

Qt::Alignment ot::OTQtConverter::toQt(ot::Alignment _alignment) {
	switch (_alignment)
	{
	case ot::AlignCenter: return Qt::AlignCenter;
	case ot::AlignTop: return Qt::AlignTop | Qt::AlignHCenter;
	case ot::AlignTopRight: return Qt::AlignTop | Qt::AlignRight;
	case ot::AlignRight: return Qt::AlignRight | Qt::AlignVCenter;
	case ot::AlignBottomRight: return Qt::AlignBottom | Qt::AlignRight;
	case ot::AlignBottom: return Qt::AlignBottom | Qt::AlignHCenter;
	case ot::AlignBottomLeft: return Qt::AlignBottom | Qt::AlignLeft;
	case ot::AlignLeft: return Qt::AlignLeft | Qt::AlignVCenter;
	case ot::AlignTopLeft: return Qt::AlignTop | Qt::AlignLeft;
	default:
		OT_LOG_EA("Unknown alignment");
		return Qt::AlignCenter;
	}
}

QPoint ot::OTQtConverter::toQt(const ot::Point2D& _pt) {
	return QPoint(_pt.x(), _pt.y());
}

QPointF ot::OTQtConverter::toQt(const ot::Point2DF& _pt) {
	return QPointF(_pt.x(), _pt.y());
}

QPointF ot::OTQtConverter::toQt(const ot::Point2DD& _pt) {
	return QPointF(_pt.x(), _pt.y());
}