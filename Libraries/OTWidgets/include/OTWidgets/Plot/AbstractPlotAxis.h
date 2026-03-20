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
#include "OTGui/Plot/Plot1DAxisCfg.h"
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_plot.h>
#include <qwt_polar.h>

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	//! @brief Abstract base class for plot axes.
	//! It provides the common interface for Cartesian and Polar plot axes.
	class OT_WIDGETS_API_EXPORT AbstractPlotAxis {
		OT_DECL_NOCOPY(AbstractPlotAxis)
		OT_DECL_NODEFAULT(AbstractPlotAxis)
	public:

		//! @brief Enumeration for axis identification.
		enum AxisID : int32_t
		{
			yLeft = 0,		//! @brief Left Y-axis (Cartesian) or Radial axis (Polar).
			yRight = 1,		//! @brief Right Y-axis (Cartesian) or Angular axis (Polar).
			xBottom = 2,	//! @brief Bottom X-axis (Cartesian) or not used in Polar plots.
			xTop = 3		//! @brief Top X-axis (Cartesian) or not used in Polar plots.
		};

		explicit AbstractPlotAxis(AxisID _id);

		virtual ~AbstractPlotAxis();

		//! @brief Updates the axis properties (title, scale type, auto-scaling, display format, ...) on the plot.
		virtual void updateAxis() = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Returns the identifier of the axis.
		AxisID getAxisID() const { return m_id; };

		//! @brief Returns the corresponding Qwt axis identifier for Cartesian plots. 
		QwtPlot::Axis getCartesianAxisID() const;

		//! @brief Returns the corresponding Qwt axis identifier for Polar plots.
		QwtPolar::Axis getPolarAxisID() const;

		//! @brief Sets the title of the axis.
		//! Call updateAxis() to apply the change to the plot.
		//! @param _title The new title for the axis.
		void setTitle(const QString& _title) { m_title = _title; };
		const QString& getTitle() const { return m_title; }

		//! @brief Enables or disables auto-scaling for the axis.
		//! Call updateAxis() to apply the change to the plot.
		//! @param _isAutoScale True to enable auto-scaling, false to disable.
		void setIsAutoScale(bool _isAutoScale);
		bool getIsAutoScale() const;

		//! @brief Enables or disables logarithmic scaling for the axis.
		//! Call updateAxis() to apply the change to the plot.
		//! @param _isLogScale True to enable logarithmic scaling, false to disable.
		void setIsLogScale(bool _isLogScale);
		bool getIsLogScale() const;

		//! @brief Sets the minimum value of the axis scale.
		//! Call updateAxis() to apply the change to the plot.
		//! @param _minValue The new minimum value for the axis scale.
		void setMin(double _minValue);
		double getMin() const;

		//! @brief Sets the maximum value of the axis scale.
		//! Call updateAxis() to apply the change to the plot.
		//! @param _maxValue The new maximum value for the axis scale.
		void setMax(double _maxValue);
		double getMax() const;

	protected:
		//! @brief Sets whether logarithmic scaling is currently applied to the axis.
		//! This is used internally to track the current state of the axis scaling and apply the necessary changes when updateAxis() is called.
		//! @param _isSet 
		inline void setIsLogScaleSet(bool _isSet) { m_isLogScaleSet = _isSet; };
		inline bool getIsLogScaleSet() const { return m_isLogScaleSet; };

	private:
		Plot1DAxisCfg m_config;

		AxisID m_id;
		QString m_title;
		bool m_isLogScaleSet;
	};

}