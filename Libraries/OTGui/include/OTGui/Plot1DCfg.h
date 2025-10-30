// @otlicense
// File: Plot1DCfg.h
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
#include "OTCore/Color.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/Plot1DAxisCfg.h"
#include "OTGui/WidgetViewBase.h"
#include "OTGui/NavigationTreeItemIcon.h"
#include "OTCore/ValueComparisionDefinition.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT Plot1DCfg : public WidgetViewBase {
		OT_DECL_DEFCOPY(Plot1DCfg)
		OT_DECL_DEFMOVE(Plot1DCfg)
	public:
		enum PlotType {
			Cartesian,
			Polar
		};

		enum AxisQuantity {
			Magnitude,
			Phase,
			Real,
			Imaginary,
			Complex
		};

		static std::string plotTypeToString(PlotType _type);
		static PlotType stringToPlotType(const std::string& _type);

		static std::string axisQuantityToString(AxisQuantity _quantity);
		static AxisQuantity stringToAxisQuantity(const std::string& _quantity);

		// ###########################################################################################################################################################################################################################################################################################################################

		Plot1DCfg();
		virtual ~Plot1DCfg();
		
		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		bool operator == (const Plot1DCfg& _other) const;
		bool operator != (const Plot1DCfg& _other) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setCollectionName(const std::string& _collectionName) { m_collectionName = _collectionName; };
		const std::string& getCollectionName() const { return m_collectionName; };

		void setPlotType(PlotType _type) { m_type = _type; };
		PlotType getPlotType() const { return m_type; };

		void setAxisQuantity(AxisQuantity _quantity) { m_axisQuantity = _quantity; };
		AxisQuantity getAxisQuantity() const { return m_axisQuantity; };

		void setGridVisible(bool _visible) { m_gridVisible = _visible; };
		bool getGridVisible() const { return m_gridVisible; };

		void setGridColor(const Color& _color) { m_gridColor = _color; };
		const Color& getGridColor() const { return m_gridColor; };

		void setGridLineWidth(double _width) { m_gridWidth = _width; };
		double getGridLineWidth() const { return m_gridWidth; };

		void setHidden(bool _isHidden) { m_isHidden = _isHidden; };
		bool getHidden() const { return m_isHidden; };

		void setLegendVisible(bool _isVisible) { m_legendVisible = _isVisible; };
		bool getLegendVisible() const { return m_legendVisible; };

		void setOldTreeIcons(const NavigationTreeItemIcon& _icons) { m_treeIcons = _icons; };
		const NavigationTreeItemIcon& getOldTreeIcons() const { return m_treeIcons; };

		void setXAxis(const Plot1DAxisCfg& _xAxis) { m_xAxis = _xAxis; };
		const Plot1DAxisCfg& getXAxis() const { return m_xAxis; };

		void setXAxisMin(double _min) { m_xAxis.setMin(_min); };
		double getXAxisMin() const { return m_xAxis.getMin(); };

		void setXAxisMax(double _max) { m_xAxis.setMax(_max); };
		double getXAxisMax() const { return m_xAxis.getMax(); };

		void setXAxisIsLogScale(bool _logScaleEnabled) { m_xAxis.setIsLogScale(_logScaleEnabled); };
		bool getXAxisIsLogScale() const { return m_xAxis.getIsLogScale(); };

		void setXAxisIsAutoScale(bool _autoScaleEnabled) { m_xAxis.setIsAutoScale(_autoScaleEnabled); };
		bool getXAxisIsAutoScale() const { return m_xAxis.getIsAutoScale(); };

		void setYAxis(const Plot1DAxisCfg& _yAxis) { m_yAxis = _yAxis; };
		const Plot1DAxisCfg& getYAxis() const { return m_yAxis; };

		void setYAxisMin(double _min) { m_yAxis.setMin(_min); };
		double getYAxisMin() const { return m_yAxis.getMin(); };

		void setYAxisMax(double _max) { m_yAxis.setMax(_max); };
		double getYAxisMax() const { return m_yAxis.getMax(); };

		void setYAxisIsLogScale(bool _logScaleEnabled) { m_yAxis.setIsLogScale(_logScaleEnabled); };
		bool getYAxisIsLogScale() const { return m_yAxis.getIsLogScale(); };

		void setYAxisIsAutoScale(bool _autoScaleEnabled) { m_yAxis.setIsAutoScale(_autoScaleEnabled); };
		bool getYAxisIsAutoScale() const { return m_yAxis.getIsAutoScale(); };

		std::string getXAxisParameter() const { return m_xAxisParameter; };
		void setXAxisParameter(const std::string _parameterName) { m_xAxisParameter = _parameterName; };

		bool getXLabelAxisAutoDetermine() const { return m_xAxis.getAutoDetermineAxisLabel(); };
		void setXLabelAxisAutoDetermine(bool _autoDetermine) { m_xAxis.setAutoDetermineAxisLabel(_autoDetermine); };

		bool getYLabelAxisAutoDetermine() const { return m_yAxis.getAutoDetermineAxisLabel(); };
		void setYLabelAxisAutoDetermine(bool _autoDetermine) { m_yAxis.setAutoDetermineAxisLabel(_autoDetermine); };

		const std::string& getAxisLabelY() const { return m_yAxis.getAxisLabel(); };
		void setAxisLabelY(const std::string& _label) { m_yAxis.setAxisLabel(_label); };

		const std::string& getAxisLabelX() const { return m_xAxis.getAxisLabel(); };
		void setAxisLabelX(const std::string& _label) { m_xAxis.setAxisLabel(_label); };

		const std::list<ValueComparisionDefinition>& getQueries() { return m_queries; };
		void setQueries(const std::list<ValueComparisionDefinition>& _queries) { m_queries = _queries; };
		void setQueries(std::list<ValueComparisionDefinition>&& _queries) { m_queries = std::move(_queries); };

		void setUseLimitNbOfCurves(bool _useLimit) { m_useLimit = _useLimit; };
		bool getUseLimitNbOfCurves() const { return m_useLimit; };

		void setLimitOfCurves(int32_t _curveLimit) { m_curveLimit = _curveLimit; };
		int32_t getLimitOfCurves() const { return m_curveLimit; };

		void setShowEntireMatrix(bool _showEntireMatrix) { m_showEntireMatrix = _showEntireMatrix; };
		bool getShowEntireMatrix() const { return m_showEntireMatrix; };

		void setShowMatrixColumnEntry(int32_t _columnEntry) { m_showMatrixColumnEntry = _columnEntry; };
		int32_t getShowMatrixColumnEntry() { return m_showMatrixColumnEntry; };
		
		void setShowMatrixRowEntry(int32_t _rowEntry) { m_showMatrixRowEntry = _rowEntry; };
		int32_t getShowMatrixRowEntry() { return m_showMatrixRowEntry; };

	private:
		std::string m_collectionName;
		PlotType m_type = Plot1DCfg::Cartesian;
		AxisQuantity m_axisQuantity = Plot1DCfg::Real;

		std::string m_xAxisParameter;

		bool m_gridVisible = true;
		ot::Color m_gridColor;
		double m_gridWidth = 1.;

		bool m_showEntireMatrix = true;
		int32_t m_showMatrixColumnEntry = 1;
		int32_t m_showMatrixRowEntry = 1;

		bool m_isHidden = false;
		bool m_legendVisible = true;
		bool m_useLimit = true;
		int32_t m_curveLimit = 25;

		NavigationTreeItemIcon m_treeIcons;
		std::list<ValueComparisionDefinition> m_queries;

		Plot1DAxisCfg m_xAxis;
		Plot1DAxisCfg m_yAxis;
	};
}