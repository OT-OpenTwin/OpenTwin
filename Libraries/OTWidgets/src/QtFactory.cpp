//! @file Painter2DFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/RadialGradientPainter2D.h"
#include "OTWidgets/QtFactory.h"

QColor ot::QtFactory::toColor(const ot::Color& _color) {
	return QColor(_color.r(), _color.g(), _color.b(), _color.a());
}

QColor ot::QtFactory::toColor(const ot::ColorF& _color) {
	return QColor((int)(_color.r() * 255.f), (int)(_color.g() * 255.f), (int)(_color.b() * 255.f), (int)(_color.a() * 255.f));
}

Qt::Alignment ot::QtFactory::toAlignment(ot::Alignment _alignment) {
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

QPoint ot::QtFactory::toPoint(const ot::Point2D& _pt) {
	return QPoint(_pt.x(), _pt.y());
}

QPointF ot::QtFactory::toPoint(const ot::Point2DF& _pt) {
	return QPointF(_pt.x(), _pt.y());
}

QPointF ot::QtFactory::toPoint(const ot::Point2DD& _pt) {
	return QPointF(_pt.x(), _pt.y());
}

QSize ot::QtFactory::toSize(const ot::Size2D& _s) {
	return QSize(_s.width(), _s.height());
}

QSizeF ot::QtFactory::toSize(const ot::Size2DF& _s) {
	return QSizeF(_s.width(), _s.height());
}

QSizeF ot::QtFactory::toSize(const ot::Size2DD& _s) {
	return QSizeF(_s.width(), _s.height());
}

QGradient::Spread ot::QtFactory::toGradientSpread(ot::GradientSpread _spread) {
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

QBrush ot::QtFactory::toBrush(const ot::Painter2D* _painter) {
	if (!_painter) {
		OT_LOG_W("Painter is 0. Ignoring");
		return QBrush();
	}
	
	if (_painter->getFactoryKey() == OT_FactoryKey_FillPainter2D) {
		const FillPainter2D* painter = dynamic_cast<const FillPainter2D*>(_painter);
		OTAssertNullptr(painter);
		return QBrush(QColor(QtFactory::toColor(painter->color())));
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_LinearGradientPainter2D) {
		const LinearGradientPainter2D* painter = dynamic_cast<const LinearGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);
		
		QGradientStops stops;
		for (auto s : painter->stops()) {
			stops.append(QGradientStop(s.pos(), QtFactory::toColor(s.color())));
		}

		QLinearGradient grad(QtFactory::toPoint(painter->start()), QtFactory::toPoint(painter->finalStop()));
		grad.setSpread(QtFactory::toGradientSpread(painter->spread()));
		grad.setCoordinateMode(QGradient::ObjectBoundingMode);
		grad.setStops(stops);
		
		return grad;
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_RadialGradientPainter2D) {
		const RadialGradientPainter2D* painter = dynamic_cast<const RadialGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);

		QGradientStops stops;
		for (auto s : painter->stops()) {
			stops.append(QGradientStop(s.pos(), ot::QtFactory::toColor(s.color())));
		}

		QRadialGradient grad(ot::QtFactory::toPoint(painter->centerPoint()), painter->centerRadius());
		if (painter->isFocalPointSet()) {
			grad.setFocalPoint(ot::QtFactory::toPoint(painter->focalPoint()));
			grad.setFocalRadius(painter->focalRadius());
		}
		grad.setSpread(ot::QtFactory::toGradientSpread(painter->spread()));
		grad.setCoordinateMode(QGradient::ObjectBoundingMode);
		grad.setStops(stops);

		return grad;
	}
	else {
		OT_LOG_EAS("Unknown Painter2D provided \"" + _painter->getFactoryKey() + "\"");
		return QBrush();
	}
}

Qt::PenStyle ot::QtFactory::toPenStyle(LineStyle _style) {
	switch (_style)
	{
	case ot::NoLine: return Qt::NoPen;
	case ot::SolidLine: return Qt::SolidLine;
	case ot::DashLine: return Qt::DashLine;
	case ot::DotLine: return Qt::DotLine;
	case ot::DashDotLine: return Qt::DashDotLine;
	case ot::DashDotDotLine: return Qt::DashDotDotLine;
	default:
		OT_LOG_E("Unknown line style (" + std::to_string((int)_style) + ")");
		return Qt::NoPen;
	}
}

Qt::PenCapStyle ot::QtFactory::toPenCapStyle(LineCapStyle _style) {
	switch (_style)
	{
	case ot::FlatCap: return Qt::FlatCap;
	case ot::SquareCap: return Qt::SquareCap;
	case ot::RoundCap: return Qt::RoundCap;
	default:
		OT_LOG_E("Unknown cap style (" + std::to_string((int)_style) + ")");
		return Qt::SquareCap;
	}
}

Qt::PenJoinStyle ot::QtFactory::toPenJoinStyle(LineJoinStyle _style) {
	switch (_style)
	{
	case ot::MiterJoin: return Qt::MiterJoin;
	case ot::BevelJoin: return Qt::BevelJoin;
	case ot::RoundJoin: return Qt::RoundJoin;
	case ot::SvgMiterJoin: return Qt::SvgMiterJoin;
	default:
		OT_LOG_E("Unknown join style (" + std::to_string((int)_style) + ")");
		return Qt::BevelJoin;
	}
}

QPen ot::QtFactory::toPen(const Outline& _outline) {
	return QPen(toBrush(_outline.painter()), (qreal)_outline.width(), toPenStyle(_outline.style()), toPenCapStyle(_outline.cap()), toPenJoinStyle(_outline.joinStyle()));
}

QPen ot::QtFactory::toPen(const OutlineF& _outline) {
	return QPen(toBrush(_outline.painter()), _outline.width(), toPenStyle(_outline.style()), toPenCapStyle(_outline.cap()), toPenJoinStyle(_outline.joinStyle()));
}

QPainterPath ot::QtFactory::toPainterPath(const Path2DF& _path) {
	QPainterPath p;
	for (const Path2DF::PathEntry& e : _path.getEntries()) {
		p.moveTo(QtFactory::toPoint(e.start));

		switch (e.type)
		{
		case Path2D::LineType:
			p.lineTo(QtFactory::toPoint(e.stop));
			break;

		case Path2D::BerzierType:
			p.cubicTo(QtFactory::toPoint(e.control1), QtFactory::toPoint(e.control1), QtFactory::toPoint(e.control1));
			break;

		default:
			OT_LOG_E("Unknown path2D entry type");
			break;
		}
	}
	return p;
}