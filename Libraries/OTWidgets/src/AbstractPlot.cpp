//! @file AbstractPlot.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/AbstractPlot.h"

ot::AbstractPlot::AbstractPlot(Plot* _ownerPlot)
	: m_axisXBottom(nullptr), m_axisXTop(nullptr), m_axisYLeft(nullptr), m_axisYRight(nullptr), m_owner(_ownerPlot)
{}

ot::AbstractPlot::~AbstractPlot() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Plot

void ot::AbstractPlot::setPlotLegendVisible(bool _isVisible, bool _repaint) {
	m_config.setLegendVisible(_isVisible);
	if (_repaint) {
		this->updateLegend(); 
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Axis

void ot::AbstractPlot::setPlotAxis(AbstractPlotAxis * _axisXBottom, AbstractPlotAxis * _axisXTop, AbstractPlotAxis * _axisYLeft, AbstractPlotAxis * _axisYRight) {
	m_axisXBottom = _axisXBottom;
	m_axisXTop = _axisXTop;
	m_axisYLeft = _axisYLeft;
	m_axisYRight = _axisYRight;

	OTAssertNullptr(m_axisXBottom);
	OTAssertNullptr(m_axisXTop);
	OTAssertNullptr(m_axisYLeft);
	OTAssertNullptr(m_axisYRight);
}

ot::AbstractPlotAxis * ot::AbstractPlot::getPlotAxis(AbstractPlotAxis::AxisID _id) {
	switch (_id)
	{
	case AbstractPlotAxis::yLeft:
		OTAssertNullptr(m_axisYLeft);
		return m_axisYLeft;

	case AbstractPlotAxis::yRight:
		OTAssertNullptr(m_axisYRight);
		return m_axisYRight;

	case AbstractPlotAxis::xBottom:
		OTAssertNullptr(m_axisXBottom);
		return m_axisXBottom;

	case AbstractPlotAxis::xTop:
		OTAssertNullptr(m_axisXTop);
		return m_axisXTop;

	default:
		OT_LOG_EAS("Invalid plot axis (" + std::to_string(_id) + ")");
		return nullptr;
	}
}

void ot::AbstractPlot::setPlotAxisTitle(AbstractPlotAxis::AxisID _axis, const QString & _title) {
	this->getPlotAxis(_axis)->setTitle(_title);
}

void ot::AbstractPlot::setPlotAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _isAutoScale) {
	this->getPlotAxis(_axis)->setIsAutoScale(_isAutoScale);
}

void ot::AbstractPlot::setPlotAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _isLogScale) {
	this->getPlotAxis(_axis)->setIsLogScale(_isLogScale);
}

void ot::AbstractPlot::setPlotAxisMin(AbstractPlotAxis::AxisID _axis, double _minValue) {
	this->getPlotAxis(_axis)->setMin(_minValue);
}

void ot::AbstractPlot::setPlotAxisMax(AbstractPlotAxis::AxisID _axis, double _maxValue) {
	this->getPlotAxis(_axis)->setMax(_maxValue);
}

void ot::AbstractPlot::repaintPlotAxis(AbstractPlotAxis::AxisID _axis) {
	this->getPlotAxis(_axis)->updateAxis();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Grid

void ot::AbstractPlot::setPlotGridVisible(bool _isVisible, bool _repaint) {
	m_config.setGridVisible(_isVisible);
	if (_repaint) {
		this->updateGrid();
	}
}

void ot::AbstractPlot::setPlotGridColor(const Color& _color, bool _repaint) {
	m_config.setGridColor(_color);
	if (_repaint) {
		this->updateGrid();
	}
}

void ot::AbstractPlot::setPlotGridLineWidth(double _width, bool _repaint) {
	m_config.setGridLineWidth(_width);
	if (_repaint) {
		this->updateGrid();
	}
}
