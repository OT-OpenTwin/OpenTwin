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
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTGui/Plot/Plot1DAxisCfg.h"
#include "OTGui/Widgets/NavigationTreeItemIcon.h"
#include "OTGui/Widgets/WidgetViewBase.h"
#include "OTGui/Painter/Painter2DContainer.h"

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

		static std::string toString(PlotType _type);
		static PlotType stringToPlotType(const std::string& _type);

		static std::list<std::string> getPlotTypeStringList();

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

		void setGridVisible(bool _visible) { m_gridVisible = _visible; };
		bool getGridVisible() const { return m_gridVisible; };

		//! @brief Set the grid color painter.
		//! The config takes ownership of the painter.
		//! @param _painter The painter to use for the grid color.
		void setGridColor(Painter2D* _painter) { m_gridColor.setPainter(_painter); };
		const Painter2D* getGridColor() const { return *m_gridColor; };

		void setGridLineWidth(double _width) { m_gridWidth = _width; };
		double getGridLineWidth() const { return m_gridWidth; };

		void setHidden(bool _isHidden) { m_isHidden = _isHidden; };
		bool getHidden() const { return m_isHidden; };

		void setLegendVisible(bool _isVisible) { m_legendVisible = _isVisible; };
		bool getLegendVisible() const { return m_legendVisible; };

		void setOldTreeIcons(const NavigationTreeItemIcon& _icons) { m_treeIcons = _icons; };
		const NavigationTreeItemIcon& getOldTreeIcons() const { return m_treeIcons; };

		void setXAxis(const Plot1DAxisCfg& _xAxis) { m_xAxis = _xAxis; };
		void setXAxis(Plot1DAxisCfg&& _xAxis) { m_xAxis = std::move(_xAxis); };
		const Plot1DAxisCfg& getXAxis() const { return m_xAxis; };

		void setXAxisMin(double _min) { m_xAxis.setMin(_min); };
		double getXAxisMin() const { return m_xAxis.getMin(); };

		void setXAxisMax(double _max) { m_xAxis.setMax(_max); };
		double getXAxisMax() const { return m_xAxis.getMax(); };

		void setXAxisQuantity(Plot1DAxisCfg::AxisQuantity _quantity) { m_xAxis.setQuantity(_quantity); };
		Plot1DAxisCfg::AxisQuantity getXAxisQuantity() const { return m_xAxis.getQuantity(); };

		void setXAxisIsLogScale(bool _logScaleEnabled) { m_xAxis.setIsLogScale(_logScaleEnabled); };
		bool getXAxisIsLogScale() const { return m_xAxis.getIsLogScale(); };

		void setXAxisIsAutoScale(bool _autoScaleEnabled) { m_xAxis.setIsAutoScale(_autoScaleEnabled); };
		bool getXAxisIsAutoScale() const { return m_xAxis.getIsAutoScale(); };

		void setYAxis(const Plot1DAxisCfg& _yAxis) { m_yAxis = _yAxis; };
		void setYAxis(Plot1DAxisCfg&& _yAxis) { m_yAxis = std::move(_yAxis); };
		const Plot1DAxisCfg& getYAxis() const { return m_yAxis; };

		void setXAxisParameter(const std::string _parameterName) { m_xAxisParameter = _parameterName; };
		std::string getXAxisParameter() const { return m_xAxisParameter; };

		void setXAxisLabelAutoDetermine(bool _autoDetermine) { m_xAxis.setAutoDetermineAxisLabel(_autoDetermine); };
		bool getXAxisLabelAutoDetermine() const { return m_xAxis.getAutoDetermineAxisLabel(); };

		void setXAxisLabel(const std::string& _label) { m_xAxis.setAxisLabel(_label); };
		const std::string& getXAxisLabel() const { return m_xAxis.getAxisLabel(); };

		//! @brief Returns the axis label to be displayed.
		//! Will include the axis label, quantity name and quantity scaling information.
		std::string getXAxisDisplayLabel() const { return m_xAxis.getDisplayLabel(*this); };

		void setXAxisDisplayNumberFormat(String::DisplayNumberFormat _format) { m_xAxis.setDisplayNumberFormat(_format); };
		String::DisplayNumberFormat getXAxisDisplayNumberFormat() const { return m_xAxis.getDisplayNumberFormat(); };

		void setXAxisDisplayNumberPrecision(int32_t _precision) { m_xAxis.setDisplayNumberPrecision(_precision); };
		int32_t getXAxisDisplayNumberPrecision() const { return m_xAxis.getDisplayNumberPrecision(); };

		void setYAxisMin(double _min) { m_yAxis.setMin(_min); };
		double getYAxisMin() const { return m_yAxis.getMin(); };

		void setYAxisMax(double _max) { m_yAxis.setMax(_max); };
		double getYAxisMax() const { return m_yAxis.getMax(); };

		void setYAxisQuantity(Plot1DAxisCfg::AxisQuantity _quantity) { m_yAxis.setQuantity(_quantity); };
		Plot1DAxisCfg::AxisQuantity getYAxisQuantity() const { return m_yAxis.getQuantity(); };

		void setYAxisIsLogScale(bool _logScaleEnabled) { m_yAxis.setIsLogScale(_logScaleEnabled); };
		bool getYAxisIsLogScale() const { return m_yAxis.getIsLogScale(); };

		void setYAxisIsAutoScale(bool _autoScaleEnabled) { m_yAxis.setIsAutoScale(_autoScaleEnabled); };
		bool getYAxisIsAutoScale() const { return m_yAxis.getIsAutoScale(); };

		void setYAxisLabelAutoDetermine(bool _autoDetermine) { m_yAxis.setAutoDetermineAxisLabel(_autoDetermine); };
		bool getYAxisLabelAutoDetermine() const { return m_yAxis.getAutoDetermineAxisLabel(); };

		void setYAxisLabel(const std::string& _label) { m_yAxis.setAxisLabel(_label); };
		const std::string& getYAxisLabel() const { return m_yAxis.getAxisLabel(); };

		//! @brief Returns the axis label to be displayed.
		//! Will include the axis label, quantity name and quantity scaling information.
		std::string getYAxisDisplayLabel() const { return m_yAxis.getDisplayLabel(*this); };

		void setYAxisDisplayNumberFormat(String::DisplayNumberFormat _format) { m_yAxis.setDisplayNumberFormat(_format); };
		String::DisplayNumberFormat getYAxisDisplayNumberFormat() const { return m_yAxis.getDisplayNumberFormat(); };

		void setYAxisDisplayNumberPrecision(int32_t _precision) { m_yAxis.setDisplayNumberPrecision(_precision); };
		int32_t getYAxisDisplayNumberPrecision() const { return m_yAxis.getDisplayNumberPrecision(); };

		const std::list<ValueComparisonDescription>& getQueries() { return m_queries; };
		void setQueries(const std::list<ValueComparisonDescription>& _queries) { m_queries = _queries; };
		void setQueries(std::list<ValueComparisonDescription>&& _queries) { m_queries = std::move(_queries); };

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

		void setDataLabelX(const std::string& _dataLabelX) { m_dataLabelX = _dataLabelX; };
		const std::string& getDataLabelX() const { return m_dataLabelX; };

		void setDataLabelY(const std::string& _dataLabelY) { m_dataLabelY = _dataLabelY; };
		const std::string& getDataLabelY() const { return m_dataLabelY; };

		void setUnitLabelX(const std::string& _unitLabelX) { m_unitLabelX = _unitLabelX; };
		const std::string& getUnitLabelX() const { return m_unitLabelX; };

		void setUnitLabelY(const std::string& _unitLabelY) { m_unitLabelY = _unitLabelY; };
		const std::string& getUnitLabelY() const { return m_unitLabelY; };

	private:
		std::string m_collectionName;
		PlotType m_type = Plot1DCfg::Cartesian;

		std::string m_xAxisParameter;

		bool m_gridVisible = true;
		Painter2DContainer m_gridColor;
		double m_gridWidth = 1.;

		bool m_showEntireMatrix = true;
		int32_t m_showMatrixColumnEntry = 1;
		int32_t m_showMatrixRowEntry = 1;

		bool m_isHidden = false;
		bool m_legendVisible = true;
		bool m_useLimit = true;
		int32_t m_curveLimit = 25;

		NavigationTreeItemIcon m_treeIcons;
		std::list<ValueComparisonDescription> m_queries;

		std::string m_dataLabelX;
		std::string m_dataLabelY;

		std::string m_unitLabelX;
		std::string m_unitLabelY;

		Plot1DAxisCfg m_xAxis;
		Plot1DAxisCfg m_yAxis;
	};
}