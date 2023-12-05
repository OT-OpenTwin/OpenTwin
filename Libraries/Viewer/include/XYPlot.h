#pragma once

// Viewer header
#include "AbstractPlot.h"

// Qwt header
#include <qwt_plot_marker.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>

#include <qpoint.h>

class Plot;
class xyPlotZoomer;
class xyPlotMagnifier;
class xyPlotPanner;
class xyPlotWidgetCanvas;
class xyPlotLegend;
class xyPlotGrid;
class xyPlotPicker;
class QwtPlotCurve;

class xyPlot : public QwtPlot, public AbstractPlot{
public:
	xyPlot(Plot * _owner);
	virtual ~xyPlot();

	// ################################################################

	// Plot
	
	virtual void RepaintLegend(void) override;

	virtual void RefreshWholePlot(void) override;

	virtual void Clear(void) override;

	void setZoomerPen(const QPen & _pen);

	// ################################################################

	// Grid

	virtual void RepaintGrid(void) override;

	// ################################################################

	// Getter

	Plot * Owner(void) const { return m_owner; }

	QwtPlotCurve * findNearestCurve(const QPoint & _pos, int & _pointID);

private:

	Plot *					m_owner;
	xyPlotZoomer *			m_plotZoomer;
	xyPlotMagnifier *		m_plotMagnifier;
	xyPlotPanner *			m_plotPanner;
	xyPlotWidgetCanvas *	m_canvas;
	xyPlotLegend *			m_legend;
	xyPlotGrid *			m_grid;
	xyPlotPicker *			m_picker;

	xyPlot() = delete;
	xyPlot(xyPlot&) = delete;
	xyPlot& operator =(xyPlot&) = delete;

};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotAxis : public AbstractPlotAxis {
public:
	xyPlotAxis(AxisID _axisID, xyPlot * _plot);
	virtual ~xyPlotAxis();

	virtual void repaint(void) override;

private:
	xyPlot *		m_plot;

	xyPlotAxis() = delete;
	xyPlotAxis(xyPlotAxis&) = delete;
	xyPlotAxis& operator = (xyPlotAxis&) = delete;
};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotWidgetCanvas : public QwtPlotCanvas {
	//Q_OBJECT
public:
	xyPlotWidgetCanvas(xyPlot * _plot);
	virtual ~xyPlotWidgetCanvas();

private:
	xyPlot *	m_plot;

};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotMarker : public QwtPlotMarker {
public:
	xyPlotMarker(int _id);
	virtual ~xyPlotMarker();

	int id(void) const { return m_id; }
	
	void setStyle(const QColor & _innerColor, const QColor & _outerColor, int _size = 8, double _outerColorSize = 2.0);
	
private:

	int			m_id;
	QwtSymbol *	m_symbol;

	xyPlotMarker() = delete;
	xyPlotMarker(xyPlotMarker&) = delete;
	xyPlotMarker& operator = (xyPlotMarker&) = delete;

};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotPanner : public QwtPlotPanner
{
public:
	xyPlotPanner(xyPlot * _plot);

	virtual bool eventFilter(QObject * _object, QEvent * _event) override;

private:
	bool		m_mouseIsPressed;
};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotZoomer : public QwtPlotZoomer
{
public:
	xyPlotZoomer(xyPlot * _plot);

	virtual void widgetMousePressEvent(QMouseEvent * _event) override;

	virtual void widgetMouseDoubleClickEvent(QMouseEvent * _event) override;

	virtual void widgetMouseMoveEvent(QMouseEvent * _event) override;

	virtual void widgetWheelEvent(QWheelEvent * _event) override;

	virtual void widgetMouseReleaseEvent(QMouseEvent * _event) override;

	virtual void widgetKeyPressEvent(QKeyEvent * _event) override;

	virtual void widgetKeyReleaseEvent(QKeyEvent * _event) override;

	bool isControlPressed(void) const { return m_controlIsPressed; }

	//virtual bool eventFilter(QObject * _object, QEvent * _event);

private:
	xyPlot *		m_plot;
	bool			m_mouseMoved;
	bool			m_controlIsPressed;
};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotMagnifier : public QwtPlotMagnifier {
public:
	xyPlotMagnifier(xyPlot * _plot);

	virtual void widgetMousePressEvent(QMouseEvent * _event) override;

	virtual void widgetMouseMoveEvent(QMouseEvent * _event) override;

	virtual void widgetMouseReleaseEvent(QMouseEvent * _event) override;

	virtual void widgetWheelEvent(QWheelEvent * _wheelEvent) override;

	virtual void rescale(double _factor) override;

	xyPlotMarker * marker(void) const { return m_marker; }

private:
	QPointF			m_cursorPos;
	bool			m_rightMouseIsPressed;
	bool			m_mouseMoved;
	xyPlot *		m_plot;
	xyPlotMarker *	m_marker;

};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotLegend : public QwtLegend {
public:
	explicit xyPlotLegend(xyPlot * _owner);
	virtual ~xyPlotLegend();

private:
	xyPlot *	m_owner;

};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotGrid : public QwtPlotGrid {
public:
	xyPlotGrid();
	virtual ~xyPlotGrid();

	void setColor(const QColor & _color, bool _repaint = true);

	void setWidth(double _width, bool _repaint = true);

private:

	double		m_width;
	QColor		m_color;

};

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

class xyPlotPicker : public QwtPlotPicker {
public:
	explicit xyPlotPicker(xyPlot * _plot);
	virtual ~xyPlotPicker();

private:
	xyPlot *		m_plot;
};

