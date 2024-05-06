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

QColor ot::QtFactory::toQt(const ot::Color& _color) {
	return QColor(_color.r(), _color.g(), _color.b(), _color.a());
}

QColor ot::QtFactory::toQt(const ot::ColorF& _color) {
	return QColor((int)(_color.r() * 255.f), (int)(_color.g() * 255.f), (int)(_color.b() * 255.f), (int)(_color.a() * 255.f));
}

Qt::Alignment ot::QtFactory::toQt(ot::Alignment _alignment) {
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

QPoint ot::QtFactory::toQt(const ot::Point2D& _pt) {
	return QPoint(_pt.x(), _pt.y());
}

QPointF ot::QtFactory::toQt(const ot::Point2DF& _pt) {
	return QPointF(_pt.x(), _pt.y());
}

QPointF ot::QtFactory::toQt(const ot::Point2DD& _pt) {
	return QPointF(_pt.x(), _pt.y());
}

QSize ot::QtFactory::toQt(const ot::Size2D& _s) {
	return QSize(_s.width(), _s.height());
}

QSizeF ot::QtFactory::toQt(const ot::Size2DF& _s) {
	return QSizeF(_s.width(), _s.height());
}

QSizeF ot::QtFactory::toQt(const ot::Size2DD& _s) {
	return QSizeF(_s.width(), _s.height());
}

QGradient::Spread ot::QtFactory::toQt(ot::GradientSpread _spread) {
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

QBrush ot::QtFactory::toQt(const ot::Painter2D* _painter) {
	if (!_painter) {
		OT_LOG_W("Painter is 0. Ignoring");
		return QBrush();
	}
	
	if (_painter->getFactoryKey() == OT_FactoryKey_FillPainter2D) {
		const FillPainter2D* painter = dynamic_cast<const FillPainter2D*>(_painter);
		OTAssertNullptr(painter);
		return QBrush(QColor(QtFactory::toQt(painter->color())));
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_LinearGradientPainter2D) {
		const LinearGradientPainter2D* painter = dynamic_cast<const LinearGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);
		
		QGradientStops stops;
		for (auto s : painter->stops()) {
			stops.append(QGradientStop(s.pos(), QtFactory::toQt(s.color())));
		}

		QLinearGradient grad(QtFactory::toQt(painter->start()), QtFactory::toQt(painter->finalStop()));
		grad.setSpread(QtFactory::toQt(painter->spread()));
		grad.setCoordinateMode(QGradient::ObjectBoundingMode);
		grad.setStops(stops);
		
		return grad;
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_RadialGradientPainter2D) {
		const RadialGradientPainter2D* painter = dynamic_cast<const RadialGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);

		QGradientStops stops;
		for (auto s : painter->stops()) {
			stops.append(QGradientStop(s.pos(), ot::QtFactory::toQt(s.color())));
		}

		QRadialGradient grad(ot::QtFactory::toQt(painter->centerPoint()), painter->centerRadius());
		if (painter->isFocalPointSet()) {
			grad.setFocalPoint(ot::QtFactory::toQt(painter->focalPoint()));
			grad.setFocalRadius(painter->focalRadius());
		}
		grad.setSpread(ot::QtFactory::toQt(painter->spread()));
		grad.setCoordinateMode(QGradient::ObjectBoundingMode);
		grad.setStops(stops);

		return grad;
	}
	else {
		OT_LOG_EAS("Unknown Painter2D provided \"" + _painter->getFactoryKey() + "\"");
		return QBrush();
	}
}