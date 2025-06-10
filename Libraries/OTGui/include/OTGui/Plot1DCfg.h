//! @file Plot1DCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
		Plot1DCfg(const Plot1DCfg& _other);
		virtual ~Plot1DCfg();
		
		Plot1DCfg& operator = (const Plot1DCfg& _other) = default;

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		bool operator == (const Plot1DCfg& _other) const;
		bool operator != (const Plot1DCfg& _other) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setProjectName(const std::string& _projectName) { m_projectName = _projectName; };
		const std::string& getProjectName(void) const { return m_projectName; };

		void setPlotType(PlotType _type) { m_type = _type; };
		PlotType getPlotType(void) const { return m_type; };

		void setAxisQuantity(AxisQuantity _quantity) { m_axisQuantity = _quantity; };
		AxisQuantity getAxisQuantity(void) const { return m_axisQuantity; };

		void setGridVisible(bool _visible) { m_gridVisible = _visible; };
		bool getGridVisible(void) const { return m_gridVisible; };

		void setGridColor(const Color& _color) { m_gridColor = _color; };
		const Color& getGridColor(void) const { return m_gridColor; };

		void setGridLineWidth(double _width) { m_gridWidth = _width; };
		double getGridLineWidth(void) const { return m_gridWidth; };

		void setHidden(bool _isHidden) { m_isHidden = _isHidden; };
		bool getHidden(void) const { return m_isHidden; };

		void setLegendVisible(bool _isVisible) { m_legendVisible = _isVisible; };
		bool getLegendVisible(void) const { return m_legendVisible; };

		void setOldTreeIcons(const NavigationTreeItemIcon& _icons) { m_treeIcons = _icons; };
		const NavigationTreeItemIcon& getOldTreeIcons(void) const { return m_treeIcons; };

		void setXAxis(const Plot1DAxisCfg& _xAxis) { m_xAxis = _xAxis; };
		const Plot1DAxisCfg& getXAxis(void) const { return m_xAxis; };

		void setXAxisMin(double _min) { m_xAxis.setMin(_min); };
		double getXAxisMin(void) const { return m_xAxis.getMin(); };

		void setXAxisMax(double _max) { m_xAxis.setMax(_max); };
		double getXAxisMax(void) const { return m_xAxis.getMax(); };

		void setXAxisIsLogScale(bool _logScaleEnabled) { m_xAxis.setIsLogScale(_logScaleEnabled); };
		bool getXAxisIsLogScale(void) const { return m_xAxis.getIsLogScale(); };

		void setXAxisIsAutoScale(bool _autoScaleEnabled) { m_xAxis.setIsAutoScale(_autoScaleEnabled); };
		bool getXAxisIsAutoScale(void) const { return m_xAxis.getIsAutoScale(); };

		void setYAxis(const Plot1DAxisCfg& _yAxis) { m_yAxis = _yAxis; };
		const Plot1DAxisCfg& getYAxis(void) const { return m_yAxis; };

		void setYAxisMin(double _min) { m_yAxis.setMin(_min); };
		double getYAxisMin(void) const { return m_yAxis.getMin(); };

		void setYAxisMax(double _max) { m_yAxis.setMax(_max); };
		double getYAxisMax(void) const { return m_yAxis.getMax(); };

		void setYAxisIsLogScale(bool _logScaleEnabled) { m_yAxis.setIsLogScale(_logScaleEnabled); };
		bool getYAxisIsLogScale(void) const { return m_yAxis.getIsLogScale(); };

		void setYAxisIsAutoScale(bool _autoScaleEnabled) { m_yAxis.setIsAutoScale(_autoScaleEnabled); };
		bool getYAxisIsAutoScale(void) const { return m_yAxis.getIsAutoScale(); };

		std::string getXAxisParameter() const { return m_xAxisParameter; }
		void setXAxisParameter(const std::string _parameterName) { m_xAxisParameter = _parameterName; }

		bool getXLabelAxisAutoDetermine() const {return m_xAxis.getAutoDetermineAxisLabel(); }
		void setXLabelAxisAutoDetermine(bool _autoDetermine) { m_xAxis.setAutoDetermineAxisLabel(_autoDetermine); }

		bool getYLabelAxisAutoDetermine() const { return m_yAxis.getAutoDetermineAxisLabel(); }
		void setYLabelAxisAutoDetermine(bool _autoDetermine) { m_yAxis.setAutoDetermineAxisLabel(_autoDetermine); }

		const std::string& getAxisLabelY() const {return m_yAxis.getAxisLabel(); }
		void setAxisLabelY(const std::string& _label) { m_yAxis.setAxisLabel(_label); }

		const std::string& getAxisLabelX() const { return m_xAxis.getAxisLabel(); }
		void setAxisLabelX(const std::string& _label) {m_xAxis.setAxisLabel(_label);}


		const std::list<ValueComparisionDefinition>& getQueries() { return m_queries; }
		void setQueries(std::list<ValueComparisionDefinition>& _queries);

		void setUseLimitNbOfCurves(bool _useLimit) { m_useLimit = _useLimit; }
		bool getUseLimitNbOfCurves() const{ return m_useLimit; }

		void setLimitOfCurves(int32_t _curveLimit) { m_curveLimit = _curveLimit; }
		int32_t getLimitOfCurves() const { return m_curveLimit; }



	private:
		std::string m_projectName;
		PlotType m_type = Plot1DCfg::Cartesian;
		AxisQuantity m_axisQuantity = Plot1DCfg::Real;

		std::string m_xAxisParameter;

		bool m_gridVisible = true;
		ot::Color m_gridColor;
		double m_gridWidth = 1.;

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