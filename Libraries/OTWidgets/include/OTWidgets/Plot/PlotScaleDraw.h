// @otlicense
// File: PlotScaleDraw.h
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
#include "OTCore/String.h"
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_scale_draw.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PlotScaleDraw : public QwtScaleDraw
	{
		OT_DECL_NOCOPY(PlotScaleDraw)
		OT_DECL_NOMOVE(PlotScaleDraw)
	public:
		explicit PlotScaleDraw();
		virtual ~PlotScaleDraw() = default;

		virtual QwtText label(double _value) const override;

		void setNumberFormat(String::DisplayNumberFormat _format) { m_numberFormat = _format; };
		String::DisplayNumberFormat getNumberFormat() const { return m_numberFormat; };

		void setNumberPrecision(int _precision) { m_numberPrecision = _precision; };
		int getNumberPrecision() const { return m_numberPrecision; };

	private:
		String::DisplayNumberFormat m_numberFormat;
		int m_numberPrecision;
	};

}