#include "stdafx.h"

#include "AbstractPlot.h"

AbstractPlotAxis::AbstractPlotAxis(AxisID _id)
	: m_id{ _id }, m_isLogScaleLast(false), m_isAutoScale(true), m_isLogScale(false), m_maxValue(1.0), m_minValue(0.0) {}

AbstractPlotAxis::~AbstractPlotAxis() {}

// #############################################################################

void AbstractPlotAxis::setTitle(const QString & _title) {
	m_title = _title;
}

void AbstractPlotAxis::setAutoScale(bool _isAutoScale) {
	m_isAutoScale = _isAutoScale;
}

void AbstractPlotAxis::setLogScale(bool _isLogScale) {
	m_isLogScale = _isLogScale;
}

void AbstractPlotAxis::setMin(double _minValue) {
	m_minValue = _minValue;
}

void AbstractPlotAxis::setMax(double _maxValue) {
	m_maxValue = _maxValue;
}

QwtPlot::Axis AbstractPlotAxis::xyAxisID(void) const {
	switch (m_id)
	{
	case AbstractPlotAxis::yLeft: return QwtPlot::yLeft;
	case AbstractPlotAxis::yRight: return QwtPlot::yRight;
	case AbstractPlotAxis::xBottom: return QwtPlot::xBottom;
	case AbstractPlotAxis::xTop: return QwtPlot::xTop;
	default:
		assert(0);	// Not implemented axis
		return QwtPlot::xBottom;
	}
}

QwtPolar::Axis AbstractPlotAxis::polarAxisID(void) const {
	switch (m_id)
	{
	case AbstractPlotAxis::yLeft: return QwtPolar::AxisLeft;
	case AbstractPlotAxis::yRight: return QwtPolar::AxisRight;
	case AbstractPlotAxis::xBottom: return QwtPolar::AxisBottom;
	case AbstractPlotAxis::xTop: return QwtPolar::AxisTop;
	default:
		assert(0);	// Not implemented axis
		return QwtPolar::AxisBottom;
	}
}

// ###########################################################################################

// ###########################################################################################

// ###########################################################################################

PlotConfiguration::PlotConfiguration()
	: gridIsVisible(true)
{}

// ###########################################################################################

// ###########################################################################################

// ###########################################################################################

std::string AbstractPlot::plotTypeString(PlotType _type) {
	switch (_type)
	{
	case AbstractPlot::Cartesian: return "Cartesian";
	case AbstractPlot::Polar: return "Polar";
	default:
		assert(0);
		return "Cartesian";
	}
}

AbstractPlot::PlotType AbstractPlot::plotTypeFromString(const std::string& _string, bool& _isComplex) {
	_isComplex = false;
	if (_string == "Cartesian") { return AbstractPlot::Cartesian; }
	else if (_string == "Polar") { return AbstractPlot::Polar; }
	else if (_string == "Polar - Complex") { _isComplex = true; return Polar; }
	else {
		assert(0);
		return AbstractPlot::Cartesian;
	}
}

AbstractPlot::AbstractPlot(PlotType _type)
	: m_axisXBottom(nullptr), m_axisXTop(nullptr), m_axisYLeft(nullptr), m_axisYRight(nullptr), m_type(_type)
{}

AbstractPlot::~AbstractPlot() {}

// ################################################################

// Plot

void AbstractPlot::SetLegendVisible(bool _isVisible, bool _repaint) {
	m_config.legendIsVisible = _isVisible;
	if (_repaint) { RepaintLegend(); }
}

// ################################################################

// Axis

void AbstractPlot::SetAxis(AbstractPlotAxis * _axisXBottom, AbstractPlotAxis * _axisXTop, AbstractPlotAxis * _axisYLeft, AbstractPlotAxis * _axisYRight) {
	m_axisXBottom = _axisXBottom;
	m_axisXTop = _axisXTop;
	m_axisYLeft = _axisYLeft;
	m_axisYRight = _axisYRight;
}

AbstractPlotAxis * AbstractPlot::GetAxis(AbstractPlotAxis::AxisID _id) {
	switch (_id)
	{
	case AbstractPlotAxis::yLeft: assert(m_axisYLeft != nullptr); return m_axisYLeft;
	case AbstractPlotAxis::yRight: assert(m_axisYRight != nullptr); return m_axisYRight;
	case AbstractPlotAxis::xBottom: assert(m_axisXBottom != nullptr); return m_axisXBottom;
	case AbstractPlotAxis::xTop: assert(m_axisXTop != nullptr); return m_axisXTop;
	default:
		assert(0);
		return nullptr;
	}
}

void AbstractPlot::SetAxisTitle(AbstractPlotAxis::AxisID _axis, const QString & _title) {
	GetAxis(_axis)->setTitle(_title);
}

void AbstractPlot::SetAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _isAutoScale) {
	GetAxis(_axis)->setAutoScale(_isAutoScale);
}

void AbstractPlot::SetAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _isLogScale) {
	GetAxis(_axis)->setLogScale(_isLogScale);
}

void AbstractPlot::SetAxisMin(AbstractPlotAxis::AxisID _axis, double _minValue) {
	GetAxis(_axis)->setMin(_minValue);
}

void AbstractPlot::SetAxisMax(AbstractPlotAxis::AxisID _axis, double _maxValue) {
	GetAxis(_axis)->setMax(_maxValue);
}

void AbstractPlot::RepaintAxis(AbstractPlotAxis::AxisID _axis) {
	GetAxis(_axis)->repaint();
}

// ################################################################

// Grid

void AbstractPlot::SetGridVisible(bool _isVisible, bool _repaint) {
	m_config.gridIsVisible = _isVisible;
	if (_repaint) { RepaintGrid(); }
}

void AbstractPlot::SetGridColor(const QColor & _color, bool _repaint) {
	m_config.gridColor = _color;
	if (_repaint) { RepaintGrid(); }
}

void AbstractPlot::SetGridLineWidth(double _width, bool _repaint) {
	m_config.gridLineWidth = _width;
	if (_repaint) { RepaintGrid(); }
}
