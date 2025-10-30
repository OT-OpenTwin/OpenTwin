// @otlicense
// File: AbstractPlotAxis.h
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
#include "OTGui/Plot1DAxisCfg.h"
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_plot.h>
#include <qwt_polar.h>

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT AbstractPlotAxis {
		OT_DECL_NOCOPY(AbstractPlotAxis)
		OT_DECL_NODEFAULT(AbstractPlotAxis)
	public:
		enum AxisID {
			yLeft = 0,
			yRight = 1,
			xBottom = 2,
			xTop = 3
		};

		AbstractPlotAxis(AxisID _id);

		virtual ~AbstractPlotAxis();

		virtual void updateAxis(void) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setTitle(const QString& _title) { m_title = _title; };
		const QString& getTitle(void) const { return m_title; }

		void setIsAutoScale(bool _isAutoScale);
		bool getIsAutoScale(void) const;

		void setIsLogScale(bool _isLogScale);
		bool getIsLogScale(void) const;

		void setMin(double _minValue);
		double getMin(void) const;

		void setMax(double _maxValue);
		double getMax(void) const;

		AxisID getAxisID(void) const { return m_id; };
		QwtPlot::Axis getCartesianAxisID(void) const;
		QwtPolar::Axis getPolarAxisID(void) const;

		void setIsLogScaleSet(bool _isSet) { m_isLogScaleSet = _isSet; };
		bool getIsLogScaleSet(void) const { return m_isLogScaleSet; };

	private:
		Plot1DAxisCfg m_config;

		AxisID m_id;
		QString m_title;
		bool m_isLogScaleSet;
	};

}