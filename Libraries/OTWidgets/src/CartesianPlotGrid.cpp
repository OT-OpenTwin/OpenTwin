//! @file CartesianPlotGrid.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/CartesianPlotGrid.h"

ot::CartesianPlotGrid::CartesianPlotGrid() {
	this->setPen(QColor(100, 100, 100), 0.5);
}

ot::CartesianPlotGrid::~CartesianPlotGrid() {}

void ot::CartesianPlotGrid::setGrid(const QColor& _color, double _width) {
	this->setPen(_color, _width);
}

void ot::CartesianPlotGrid::setGrid(const QPen& _pen) {
	this->setPen(_pen);
}
