/*
 *	File:		PolarPlot.cpp
 *
 *  Created on: May 17, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the QwtWrapper project.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <qwtw/Plot.h>
#include <qwtw/PolarPlot.h>

#include <qwt_scale_engine.h>

#include <qevent.h>

qwtw::PolarPlot::PolarPlot(Plot * _owner)
	: AbstractPlot(Polar), m_owner(_owner)
{
	m_grid = new PolarPlotGrid(this);
	m_legend = new PolarPlotLegend(this);
	m_magnifier = new PolarPlotMagnifier(this);
	m_panner = new PolarPlotPanner(this);

	SetAxis(
		new PolarPlotAxis(AbstractPlotAxis::xBottom, this),
		nullptr,
		new PolarPlotAxis(AbstractPlotAxis::yLeft, this),
		nullptr
	);
}

qwtw::PolarPlot::~PolarPlot() {

}

// ################################################################

// Plot

void qwtw::PolarPlot::RepaintLegend(void) {
	
}

void qwtw::PolarPlot::RefreshWholePlot(void) {
	m_axisXBottom->repaint();
	m_axisYLeft->repaint();

	replot();

	
}

void qwtw::PolarPlot::Clear(void) {

}

void qwtw::PolarPlot::setAxisValueLabels(AbstractPlotAxis::AxisID _axis, const std::map<double, QString>& _valueToLabelMap) {

}

void qwtw::PolarPlot::clearAxisValueLabels(AbstractPlotAxis::AxisID _axis) {

}


// ################################################################

// Grid

void qwtw::PolarPlot::RepaintGrid(void) {
	if (m_config.gridIsVisible) {
		m_grid->setPen(QPen(m_config.gridColor, m_config.gridLineWidth, Qt::SolidLine));
		m_grid->setAxisPen(QwtPolar::Axis::AxisAzimuth, QPen(m_config.gridColor, m_config.gridLineWidth, Qt::SolidLine));
		m_grid->setAxisPen(QwtPolar::Axis::AxisBottom, QPen(m_config.gridColor, m_config.gridLineWidth, Qt::SolidLine));
		m_grid->setAxisPen(QwtPolar::Axis::AxisLeft, QPen(m_config.gridColor, m_config.gridLineWidth, Qt::SolidLine));
		m_grid->setAxisPen(QwtPolar::Axis::AxisRight, QPen(m_config.gridColor, m_config.gridLineWidth, Qt::SolidLine));
		m_grid->setAxisPen(QwtPolar::Axis::AxisTop, QPen(m_config.gridColor, m_config.gridLineWidth, Qt::SolidLine));
	}
	else {
		m_grid->setPen(QPen(QColor(), 0.0, Qt::NoPen));
		m_grid->setAxisPen(QwtPolar::Axis::AxisAzimuth, QPen(QColor(), 0.0, Qt::NoPen));
		m_grid->setAxisPen(QwtPolar::Axis::AxisBottom, QPen(QColor(), 0.0, Qt::NoPen));
		m_grid->setAxisPen(QwtPolar::Axis::AxisLeft, QPen(QColor(), 0.0, Qt::NoPen));
		m_grid->setAxisPen(QwtPolar::Axis::AxisRight, QPen(QColor(), 0.0, Qt::NoPen));
		m_grid->setAxisPen(QwtPolar::Axis::AxisTop, QPen(QColor(), 0.0, Qt::NoPen));
	}
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

qwtw::PolarPlotAxis::PolarPlotAxis(AxisID _axisID, PolarPlot * _plot)
	: AbstractPlotAxis(_axisID), m_plot(_plot)
{
	m_plot->setScaleEngine(polarAxisID(), new QwtLinearScaleEngine());
}

qwtw::PolarPlotAxis::~PolarPlotAxis() {

}

void qwtw::PolarPlotAxis::repaint(void) {
	if (m_isLogScale && !m_isLogScaleLast) {
		m_isLogScaleLast = true;
		m_plot->setScaleEngine(polarAxisID(), new QwtLogScaleEngine());

	}
	else if (!m_isLogScale && m_isLogScaleLast) {
		m_isLogScaleLast = false;
		m_plot->setScaleEngine(polarAxisID(), new QwtLinearScaleEngine());
	}

	//NOTE, auto scale required?
	//m_plot->setAutoScale(polarAxisID());

	//NOTE, axis title?

	if (!m_isAutoScale) {
		//m_plot->setAxisMaxMajor(polarAxisID(), m_maxValue);
		//m_plot->setAxisMaxMinor(polarAxisID(), m_minValue);
	}

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

qwtw::PolarPlotGrid::PolarPlotGrid(PolarPlot * _plot)
	: m_plot(_plot)
{
	attach(m_plot);
}

qwtw::PolarPlotGrid::~PolarPlotGrid() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

qwtw::PolarPlotLegend::PolarPlotLegend(PolarPlot * _plot)
	: m_plot(_plot)
{
	m_plot->insertLegend(this);
}

qwtw::PolarPlotLegend::~PolarPlotLegend() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

qwtw::PolarPlotPanner::PolarPlotPanner(PolarPlot * _plot)
	: QwtPolarPanner(_plot->canvas()), m_plot(_plot)
{
	setMouseButton(Qt::MouseButton::MiddleButton);
	setEnabled(true);
}

qwtw::PolarPlotPanner::~PolarPlotPanner() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

qwtw::PolarPlotMagnifier::PolarPlotMagnifier(PolarPlot * _plot)
	: QwtPolarMagnifier(_plot->canvas()), m_rightMouseIsPressed(false), m_mouseMoved(false), m_plot(_plot)
{
	setMouseButton(Qt::MouseButton::NoButton);
	setEnabled(true);
	m_marker = new PolarPlotMarker(m_plot);
	m_plot->setVisible(false);
}

qwtw::PolarPlotMagnifier::~PolarPlotMagnifier() {

}

void qwtw::PolarPlotMagnifier::widgetMousePressEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = true;
		m_mouseMoved = false;
	}
	QwtPolarMagnifier::widgetMousePressEvent(_event);
}

void qwtw::PolarPlotMagnifier::widgetMouseMoveEvent(QMouseEvent * _event) {

	m_mouseMoved = true;

	if (m_rightMouseIsPressed) {
		int itemId;
		/*QwtPlotCurve * curve = m_plot->findNearestCurve(_event->pos(), itemId);

		if (curve != nullptr) {
			double x, y;
			m_plot->Owner()->findDataset(curve)->dataAt(itemId, x, y);
			m_marker->setValue(x, y);
			m_marker->setLabel(QwtText((std::to_string(x) + ", " + std::to_string(y)).c_str()));
			m_marker->setVisible(true);
			m_plot->replot();
		}*/
	}
	QwtPolarMagnifier::widgetMouseMoveEvent(_event);
}

void qwtw::PolarPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		if (m_mouseMoved) {
			m_marker->setVisible(false);
			m_plot->replot();
		}
		m_rightMouseIsPressed = false;
	}
	QwtPolarMagnifier::widgetMouseReleaseEvent(_event);
}

void qwtw::PolarPlotMagnifier::widgetWheelEvent(QWheelEvent * _wheelEvent) {
	if (_wheelEvent->modifiers() != wheelModifiers()) { return; }

	if (wheelFactor() != 0.0)
	{
		int delta = _wheelEvent->angleDelta().x() * (-1);
		double f = qPow(wheelFactor(),
			qAbs(delta / 120.0));

		if (delta > 0)
			f = 1 / f;

		rescale(f);
	}
}

void qwtw::PolarPlotMagnifier::rescale(double _factor) {
	QwtPolarMagnifier::rescale(_factor);
}

// ###############################################################################

// ###############################################################################

// ###############################################################################

qwtw::PolarPlotMarker::PolarPlotMarker(PolarPlot * _plot)
	: m_plot(_plot)
{
	attach(m_plot);
}

qwtw::PolarPlotMarker::~PolarPlotMarker() {

}