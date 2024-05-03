//! @file OTQtConverter.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/OTQtConverter.h"

QColor ot::OTQtConverter::toQt(const ot::Color& _color) {
	return QColor(_color.r(), _color.g(), _color.b(), _color.a());
}

QColor ot::OTQtConverter::toQt(const ot::ColorF& _color) {
	return QColor((int)(_color.r() * 255.f), (int)(_color.g() * 255.f), (int)(_color.b() * 255.f), (int)(_color.a() * 255.f));
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

QSize ot::OTQtConverter::toQt(const ot::Size2D& _s) {
	return QSize(_s.width(), _s.height());
}

QSizeF ot::OTQtConverter::toQt(const ot::Size2DF& _s) {
	return QSizeF(_s.width(), _s.height());
}

QSizeF ot::OTQtConverter::toQt(const ot::Size2DD& _s) {
	return QSizeF(_s.width(), _s.height());
}

QGradient::Spread ot::OTQtConverter::toQt(ot::GradientSpread _spread) {
	switch (_spread)
	{
	case ot::PadSpread: return QGradient::PadSpread;
	case ot::RepeatSpread: return QGradient::RepeatSpread;
	case ot::ReflectSpread: return QGradient::ReflectSpread;
	default:
		OT_LOG_EA("Unknown gradient spread");
		return QGradient::PadSpread;
	}
}