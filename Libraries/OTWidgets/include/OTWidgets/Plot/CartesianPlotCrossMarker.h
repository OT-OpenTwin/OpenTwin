// @otlicense
// File: CartesianPlotCrossMarker.h
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

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_plot_marker.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CartesianPlotCrossMarker : public QwtPlotMarker {
		OT_DECL_NOCOPY(CartesianPlotCrossMarker)
	public:
		CartesianPlotCrossMarker();
		virtual ~CartesianPlotCrossMarker();

		void setStyle(const QColor& _innerColor, const QColor& _outerColor, int _size = 8, double _outerColorSize = 2.0);

	private:
		QwtSymbol* m_symbol;
	};

}