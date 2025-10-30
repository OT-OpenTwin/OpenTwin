// @otlicense
// File: PolarPlotCurve.h
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
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_polar_curve.h>

// Qt header
#include <QtGui/qpen.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PolarPlotCurve : public QwtPolarCurve {
	public:

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor

		PolarPlotCurve(const QString& _title = QString());

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setOutlinePen(const QPen& _pen) { m_outlinePen = _pen; };
		const QPen& getOutlinePen() const { return m_outlinePen; };

		void setPointInterval(int _interval) { m_pointInterval = _interval; };
		int getPointInterval() const { return m_pointInterval; };

	private:
		QPen m_outlinePen;
		int m_pointInterval;
	};

}