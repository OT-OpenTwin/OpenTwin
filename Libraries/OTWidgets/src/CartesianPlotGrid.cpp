// @otlicense
// File: CartesianPlotGrid.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
