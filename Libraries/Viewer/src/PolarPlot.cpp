#include "stdafx.h"

#include "polarPlot.h"
#include "Plot.h"

#include <qwt_scale_engine.h>

#include <qevent.h>

polarPlot::polarPlot(Plot * _owner)
	: AbstractPlot(Polar), m_owner(_owner)
{
	m_grid = new polarPlotGrid(this);
	m_legend = new polarPlotLegend(this);
	m_magnifier = new polarPlotMagnifier(this);
	m_panner = new polarPlotPanner(this);

	SetAxis(
		new polarPlotAxis(AbstractPlotAxis::xBottom, this),
		nullptr,
		new polarPlotAxis(AbstractPlotAxis::yLeft, this),
		nullptr
	);
}

polarPlot::~polarPlot() {

}

// ################################################################

// Plot

void polarPlot::RepaintLegend(void) {
	
}

void polarPlot::RefreshWholePlot(void) {
	m_axisXBottom->repaint();
	m_axisYLeft->repaint();

	replot();

	
}

void polarPlot::Clear(void) {

}

// ################################################################

// Grid

void polarPlot::RepaintGrid(void) {
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

polarPlotAxis::polarPlotAxis(AxisID _axisID, polarPlot * _plot)
	: AbstractPlotAxis(_axisID), m_plot(_plot)
{
	m_plot->setScaleEngine(polarAxisID(), new QwtLinearScaleEngine());
}

polarPlotAxis::~polarPlotAxis() {

}

void polarPlotAxis::repaint(void) {
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

polarPlotGrid::polarPlotGrid(polarPlot * _plot)
	: m_plot(_plot)
{
	attach(m_plot);
}

polarPlotGrid::~polarPlotGrid() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

polarPlotLegend::polarPlotLegend(polarPlot * _plot)
	: m_plot(_plot)
{
	m_plot->insertLegend(this);
}

polarPlotLegend::~polarPlotLegend() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

polarPlotPanner::polarPlotPanner(polarPlot * _plot)
	: QwtPolarPanner(_plot->canvas()), m_plot(_plot)
{
	setMouseButton(Qt::MouseButton::MiddleButton);
	setEnabled(true);
}

polarPlotPanner::~polarPlotPanner() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

polarPlotMagnifier::polarPlotMagnifier(polarPlot * _plot)
	: QwtPolarMagnifier(_plot->canvas()), m_rightMouseIsPressed(false), m_mouseMoved(false), m_plot(_plot)
{
	setMouseButton(Qt::MouseButton::NoButton);
	setEnabled(true);
	m_marker = new polarPlotMarker(m_plot);
	m_plot->setVisible(false);
}

polarPlotMagnifier::~polarPlotMagnifier() {

}

void polarPlotMagnifier::widgetMousePressEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = true;
		m_mouseMoved = false;
	}
	QwtPolarMagnifier::widgetMousePressEvent(_event);
}

void polarPlotMagnifier::widgetMouseMoveEvent(QMouseEvent * _event) {

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

void polarPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		if (m_mouseMoved) {
			m_marker->setVisible(false);
			m_plot->replot();
		}
		m_rightMouseIsPressed = false;
	}
	QwtPolarMagnifier::widgetMouseReleaseEvent(_event);
}

void polarPlotMagnifier::widgetWheelEvent(QWheelEvent * _wheelEvent) {
	if (_wheelEvent->modifiers() != wheelModifiers()) { return; }

	if (wheelFactor() != 0.0)
	{
		int delta = _wheelEvent->angleDelta().y() * (-1);
		double f = qPow(wheelFactor(),
			qAbs(delta / 120.0));

		if (delta > 0)
			f = 1 / f;

		rescale(f);
	}
}

void polarPlotMagnifier::rescale(double _factor) {
	QwtPolarMagnifier::rescale(_factor);
}

// ###############################################################################

// ###############################################################################

// ###############################################################################

polarPlotMarker::polarPlotMarker(polarPlot * _plot) 
	: m_plot(_plot)
{
	attach(m_plot);
}

polarPlotMarker::~polarPlotMarker() {

}