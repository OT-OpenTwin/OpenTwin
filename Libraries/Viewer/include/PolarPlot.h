#pragma once

#include "AbstractPlot.h"

#include <qwt_polar_plot.h>
#include <qwt_polar_grid.h>
#include <qwt_polar_panner.h>
#include <qwt_polar_marker.h>
#include <qwt_polar_magnifier.h>
#include <qwt_legend.h>

class Plot;
class polarPlotAxis;
class polarPlotGrid;
class polarPlotLegend;
class polarPlotPanner;
class polarPlotMagnifier;
class polarPlotMarker;

class polarPlot : public QwtPolarPlot, public AbstractPlot {
public:
	polarPlot(Plot * _owner);
	virtual ~polarPlot();

	// ################################################################

	// Plot

	virtual void RepaintLegend(void) override;

	virtual void RefreshWholePlot(void) override;

	virtual void Clear(void) override;

	// ################################################################

	// Grid

	virtual void RepaintGrid(void) override;

	// ################################################################

private:
	Plot *		m_owner;

	polarPlotGrid *			m_grid;
	polarPlotLegend *		m_legend;
	polarPlotMagnifier *	m_magnifier;
	polarPlotPanner *		m_panner;

	polarPlot() = delete;
	polarPlot(polarPlot&) = delete;
	polarPlot& operator = (polarPlot&) = delete;

};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class polarPlotAxis : public AbstractPlotAxis {
public:
	polarPlotAxis(AxisID _axisID, polarPlot * _plot);
	virtual ~polarPlotAxis();

	virtual void repaint(void) override;

private:
	polarPlot *		m_plot;

	polarPlotAxis() = delete;
	polarPlotAxis(polarPlotAxis&) = delete;
	polarPlotAxis& operator = (polarPlotAxis&) = delete;
};
 
// ###############################################################################

// ###############################################################################

// ###############################################################################

class polarPlotGrid : public QwtPolarGrid {
public:
	polarPlotGrid(polarPlot * _plot);
	virtual ~polarPlotGrid();

private:
	polarPlot *			m_plot;

	polarPlotGrid() = delete;
	polarPlotGrid(polarPlotGrid&) = delete;
	polarPlotGrid& operator = (polarPlotGrid&) = delete;
};

// ###############################################################################

// ###############################################################################

// ###############################################################################

class polarPlotLegend : public QwtLegend {
public:
	polarPlotLegend(polarPlot * _plot);
	virtual ~polarPlotLegend();

private:
	polarPlot *		m_plot;
};

// ###############################################################################

// ###############################################################################

// ###############################################################################

class polarPlotPanner : public QwtPolarPanner {
public:
	polarPlotPanner(polarPlot * _plot);
	virtual ~polarPlotPanner();

private:
	polarPlot *		m_plot;
};

// ###############################################################################

// ###############################################################################

// ###############################################################################

class polarPlotMagnifier : public QwtPolarMagnifier {
public:
	polarPlotMagnifier(polarPlot * _plot);
	virtual ~polarPlotMagnifier();

	virtual void widgetMousePressEvent(QMouseEvent * _event) override;

	virtual void widgetMouseMoveEvent(QMouseEvent * _event) override;

	virtual void widgetMouseReleaseEvent(QMouseEvent * _event) override;

	virtual void widgetWheelEvent(QWheelEvent * _wheelEvent) override;

	virtual void rescale(double _factor) override;


private:
	polarPlot *			m_plot;
	QwtPolarCanvas *	m_canvas;
	bool				m_rightMouseIsPressed;
	bool				m_mouseMoved;
	polarPlotMarker *	m_marker;
};

// ###############################################################################

// ###############################################################################

// ###############################################################################

class polarPlotMarker : public QwtPolarMarker {
public:
	polarPlotMarker(polarPlot * _plot);
	virtual ~polarPlotMarker();

private:
	polarPlot *			m_plot;
};
