// @otlicense
// File: PlotScaleDraw.cpp
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
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotScaleDraw.h"

// Qwt header
#include <qwt_text.h>

ot::PlotScaleDraw::PlotScaleDraw()
	: m_numberFormat(String::DisplayNumberFormat::Auto), m_numberPrecision(3)
{

}

QwtText ot::PlotScaleDraw::label(double _value) const
{
	return QwtText(QString::fromStdString(String::numberToString(_value, m_numberFormat, m_numberPrecision)));
}
