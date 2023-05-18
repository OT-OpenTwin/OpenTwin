//! @file PainterFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/PainterFactory.h"

#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTQtWrapper/TypeConversion.h"
#include "OpenTwinCore/Logger.h"

QBrush ot::PainterFactory::brushFromPainter(ot::Painter2D* _painter) {
	if (_painter->painterType() == OT_FILLPAINTER2D_TYPE) {
		return brushFromPainter(dynamic_cast<ot::FillPainter2D*>(_painter));
	}
	else if (_painter->painterType() == OT_LINEARGRADIENTPAINTER2D_TYPE) {
		return brushFromPainter(dynamic_cast<ot::LinearGradientPainter2D*>(_painter));
	}
	else {
		OT_LOG_EAS("Unknown painter 2D type (type = \"" + _painter->painterType() + "\")");
		return QBrush();
	}
}

QBrush ot::PainterFactory::brushFromPainter(ot::FillPainter2D* _painter) {
	QBrush b(colorToQColor(_painter->color()));
	return b;
}

QBrush ot::PainterFactory::brushFromPainter(ot::LinearGradientPainter2D* _painter) {
	if (_painter->steps().size() < 2) {
		OT_LOG_EA("Can not create linear gradient with less than two points (start and end required)");
		return QBrush();
	}

	otAssert(0, "ToDo: Add linear gradient painter converter to brush");
	return QBrush();
}