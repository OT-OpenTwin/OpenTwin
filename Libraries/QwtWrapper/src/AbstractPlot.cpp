/*
 *	File:		AbstractPlot.cpp
 *
 *  Created on: May 17, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the QwtWrapper project.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <qwtw/AbstractPlot.h>

qwtw::AbstractPlotAxis::AbstractPlotAxis(AxisID _id)
	: m_id{ _id }, m_isLogScaleLast(false), m_isAutoScale(true), m_isLogScale(false), m_maxValue(1.0), m_minValue(0.0) {}

qwtw::AbstractPlotAxis::~AbstractPlotAxis() {}

// #############################################################################

void qwtw::AbstractPlotAxis::setTitle(const QString & _title) {
	m_title = _title;
}

void qwtw::AbstractPlotAxis::setAutoScale(bool _isAutoScale) {
	m_isAutoScale = _isAutoScale;
}

void qwtw::AbstractPlotAxis::setLogScale(bool _isLogScale) {
	m_isLogScale = _isLogScale;
}

void qwtw::AbstractPlotAxis::setMin(double _minValue) {
	m_minValue = _minValue;
}

void qwtw::AbstractPlotAxis::setMax(double _maxValue) {
	m_maxValue = _maxValue;
}

QwtPlot::Axis qwtw::AbstractPlotAxis::xyAxisID(void) const {
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

QwtPolar::Axis qwtw::AbstractPlotAxis::polarAxisID(void) const {
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

qwtw::PlotConfiguration::PlotConfiguration()
	: gridIsVisible(true)
{}

// ###########################################################################################

// ###########################################################################################

// ###########################################################################################

std::string qwtw::AbstractPlot::plotTypeString(PlotType _type) {
	switch (_type)
	{
	case AbstractPlot::Cartesian: return "Cartesian";
	case AbstractPlot::Polar: return "Polar";
	default:
		assert(0);
		return "Cartesian";
	}
}

qwtw::AbstractPlot::PlotType qwtw::AbstractPlot::plotTypeFromString(const std::string& _string, bool& _isComplex) {
	_isComplex = false;
	if (_string == "Cartesian") { return AbstractPlot::Cartesian; }
	else if (_string == "Polar") { return AbstractPlot::Polar; }
	else if (_string == "Polar - Complex") { _isComplex = true; return Polar; }
	else {
		assert(0);
		return AbstractPlot::Cartesian;
	}
}

qwtw::AbstractPlot::AbstractPlot(PlotType _type)
	: m_axisXBottom(nullptr), m_axisXTop(nullptr), m_axisYLeft(nullptr), m_axisYRight(nullptr), m_type(_type)
{}

qwtw::AbstractPlot::~AbstractPlot() {}

// ################################################################

// Plot

void qwtw::AbstractPlot::SetLegendVisible(bool _isVisible, bool _repaint) {
	m_config.legendIsVisible = _isVisible;
	if (_repaint) { RepaintLegend(); }
}

// ################################################################

// Axis

void qwtw::AbstractPlot::SetAxis(AbstractPlotAxis * _axisXBottom, AbstractPlotAxis * _axisXTop, AbstractPlotAxis * _axisYLeft, AbstractPlotAxis * _axisYRight) {
	m_axisXBottom = _axisXBottom;
	m_axisXTop = _axisXTop;
	m_axisYLeft = _axisYLeft;
	m_axisYRight = _axisYRight;
}

qwtw::AbstractPlotAxis * qwtw::AbstractPlot::GetAxis(AbstractPlotAxis::AxisID _id) {
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

void qwtw::AbstractPlot::SetAxisTitle(AbstractPlotAxis::AxisID _axis, const QString & _title) {
	GetAxis(_axis)->setTitle(_title);
}

void qwtw::AbstractPlot::SetAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _isAutoScale) {
	GetAxis(_axis)->setAutoScale(_isAutoScale);
}

void qwtw::AbstractPlot::SetAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _isLogScale) {
	GetAxis(_axis)->setLogScale(_isLogScale);
}

void qwtw::AbstractPlot::SetAxisMin(AbstractPlotAxis::AxisID _axis, double _minValue) {
	GetAxis(_axis)->setMin(_minValue);
}

void qwtw::AbstractPlot::SetAxisMax(AbstractPlotAxis::AxisID _axis, double _maxValue) {
	GetAxis(_axis)->setMax(_maxValue);
}

void qwtw::AbstractPlot::RepaintAxis(AbstractPlotAxis::AxisID _axis) {
	GetAxis(_axis)->repaint();
}

// ################################################################

// Grid

void qwtw::AbstractPlot::SetGridVisible(bool _isVisible, bool _repaint) {
	m_config.gridIsVisible = _isVisible;
	if (_repaint) { RepaintGrid(); }
}

void qwtw::AbstractPlot::SetGridColor(const QColor & _color, bool _repaint) {
	m_config.gridColor = _color;
	if (_repaint) { RepaintGrid(); }
}

void qwtw::AbstractPlot::SetGridLineWidth(double _width, bool _repaint) {
	m_config.gridLineWidth = _width;
	if (_repaint) { RepaintGrid(); }
}
