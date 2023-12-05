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
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/OTQtConverter.h"

QBrush ot::Painter2DFactory::brushFromPainter2D(ot::Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg) {
		ot::FillPainter2D* painter = dynamic_cast<ot::FillPainter2D*>(_painter);
		OTAssertNullptr(painter);
		return QBrush(QColor(ot::OTQtConverter::toQt(painter->color())));
	}
	else if (_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg) {
		ot::LinearGradientPainter2D* painter = dynamic_cast<ot::LinearGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);
		
		QGradientStops stops;
		for (auto s : painter->stops()) {
			stops.append(QGradientStop(s.pos(), ot::OTQtConverter::toQt(s.color())));
		}

		QLinearGradient grad(ot::OTQtConverter::toQt(painter->start()), ot::OTQtConverter::toQt(painter->finalStop()));
		grad.setSpread(ot::OTQtConverter::toQt(painter->spread()));
		grad.setCoordinateMode(QGradient::ObjectBoundingMode);
		grad.setStops(stops);
		
		return grad;
	}
	else if (_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_RadialGradientPainter2DCfg) {
		ot::RadialGradientPainter2D* painter = dynamic_cast<ot::RadialGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);

		QGradientStops stops;
		for (auto s : painter->stops()) {
			stops.append(QGradientStop(s.pos(), ot::OTQtConverter::toQt(s.color())));
		}

		QRadialGradient grad(ot::OTQtConverter::toQt(painter->centerPoint()), painter->centerRadius());
		if (painter->isFocalPointSet()) {
			grad.setFocalPoint(ot::OTQtConverter::toQt(painter->focalPoint()));
			grad.setFocalRadius(painter->focalRadius());
		}
		grad.setSpread(ot::OTQtConverter::toQt(painter->spread()));
		grad.setCoordinateMode(QGradient::ObjectBoundingMode);
		grad.setStops(stops);

		return grad;
	}
	else {
		OT_LOG_EAS("Unknown Painter2D provided \"" + _painter->simpleFactoryObjectKey() + "\"");
		return QBrush();
	}
}