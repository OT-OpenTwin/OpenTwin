//! @file Painter2DFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Painter2D.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/OTQtConverter.h"

QBrush ot::Painter2DFactory::brushFromPainter2D(ot::Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg) {
		ot::FillPainter2D* painter = dynamic_cast<ot::FillPainter2D*>(_painter);
		OTAssertNullptr(painter);
		return QBrush(QColor(ot::OTQtConverter::toQt(painter->color())));
	}
	else if (_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg) {
		ot::LinearGradientPainter2D* painter = dynamic_cast<ot::LinearGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);
		otAssert(0, "...");
		return QBrush();
	}
	else {
		OT_LOG_EAS("Unknown Painter2D provided \"" + _painter->simpleFactoryObjectKey() + "\"");
		return QBrush();
	}
}