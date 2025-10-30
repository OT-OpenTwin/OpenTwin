// @otlicense
// File: CartesianPlotCrossMarker.cpp
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
#include "OTWidgets/CartesianPlotCrossMarker.h"

// Qwt header
#include <qwt_symbol.h>

ot::CartesianPlotCrossMarker::CartesianPlotCrossMarker() : m_symbol(nullptr) {
	this->setLineStyle(QwtPlotMarker::LineStyle::Cross);
	this->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
	this->setLinePen(QPen(QColor(200, 150, 0), 0, Qt::DashDotLine));
	m_symbol = new QwtSymbol(QwtSymbol::Diamond, QColor(Qt::yellow), QColor(Qt::green), QSize(7, 7));
	this->setSymbol(m_symbol);
}

ot::CartesianPlotCrossMarker::~CartesianPlotCrossMarker() {
	delete m_symbol;
}

void ot::CartesianPlotCrossMarker::setStyle(const QColor& _innerColor, const QColor& _outerColor, int _size, double _outerColorSize) {
	m_symbol->setBrush(_innerColor);
	m_symbol->setPen(_outerColor, _outerColorSize);
	m_symbol->setSize(_size);
}
