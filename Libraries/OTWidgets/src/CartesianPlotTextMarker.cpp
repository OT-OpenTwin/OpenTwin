// @otlicense

//! @file CartesianPlotTextMarker.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/CartesianPlotTextMarker.h"

// Qwt header
#include <qwt_symbol.h>

ot::CartesianPlotTextMarker::CartesianPlotTextMarker() {
	this->setLineStyle(QwtPlotMarker::LineStyle::NoLine);
	this->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
	this->setLinePen(QPen(QColor(200, 150, 0), 0, Qt::DashDotLine));
}

ot::CartesianPlotTextMarker::~CartesianPlotTextMarker() {

}
