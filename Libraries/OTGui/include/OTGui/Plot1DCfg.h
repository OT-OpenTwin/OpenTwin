//! @file Plot1DCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/Plot1DAxisCfg.h"
#include "OTGui/NavigationTreeItemIcon.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT Plot1DCfg : public Serializable {
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
		
		Plot1DCfg& operator = (const Plot1DCfg& _other);

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

		void setUid(UID _uid) { m_uid = _uid; };
		UID getUid(void) const { return m_uid; };

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle(void) const { return m_title; };

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

		void setTreeIcons(const NavigationTreeItemIcon& _icons) { m_treeIcons = _icons; };
		const NavigationTreeItemIcon& getTreeIcons(void) const { return m_treeIcons; };

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

	private:
		UID m_uid;
		
		std::string m_name;
		std::string m_title;
		std::string m_projectName;
		PlotType m_type;
		AxisQuantity m_axisQuantity;

		bool m_gridVisible;
		ot::Color m_gridColor;
		double m_gridWidth;

		bool m_isHidden;
		bool m_legendVisible;

		NavigationTreeItemIcon m_treeIcons;

		Plot1DAxisCfg m_xAxis;
		Plot1DAxisCfg m_yAxis;
	};

}