/*
 *	File:		XYPlot.h
 *
 *  Created on: May 17, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the QwtWrapper project.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// Wrapper header
#include <qwtw/globalDatatypes.h>
#include <qwtw/AbstractPlot.h>

// Qwt header
#include <qwt_plot_marker.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>

// Qt header
#include <qpoint.h>

class QwtPlotCurve;

namespace qwtw {
	class Plot;
	class XYPlotZoomer;
	class XYPlotMagnifier;
	class XYPlotPanner;
	class XYPlotWidgetCanvas;
	class XYPlotLegend;
	class XYPlotGrid;
	class XYPlotPicker;

	class QWTW_LIB_EXPORT XYPlot : public QwtPlot, public AbstractPlot {
	public:
		XYPlot(Plot * _owner);
		virtual ~XYPlot();

		// ################################################################

		// Plot

		virtual void RepaintLegend(void) override;

		virtual void RefreshWholePlot(void) override;

		virtual void Clear(void) override;

		void setZoomerPen(const QPen & _pen);

		void setAxisValueLabels(AbstractPlotAxis::AxisID _axis, const std::map<double, QString>& _valueToLabelMap);

		void clearAxisValueLabels(AbstractPlotAxis::AxisID _axis);

		// ################################################################

		// Grid

		virtual void RepaintGrid(void) override;

		// ################################################################

		// Getter

		Plot * Owner(void) const { return m_owner; }

		QwtPlotCurve * findNearestCurve(const QPoint & _pos, int & _pointID);

	private:

		Plot *					m_owner;
		XYPlotZoomer *			m_plotZoomer;
		XYPlotMagnifier *		m_plotMagnifier;
		XYPlotPanner *			m_plotPanner;
		XYPlotWidgetCanvas *	m_canvas;
		XYPlotLegend *			m_legend;
		XYPlotGrid *			m_grid;
		XYPlotPicker *			m_picker;

		XYPlot() = delete;
		XYPlot(XYPlot&) = delete;
		XYPlot& operator =(XYPlot&) = delete;

	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotAxis : public AbstractPlotAxis {
	public:
		XYPlotAxis(AxisID _axisID, XYPlot * _plot);
		virtual ~XYPlotAxis();

		virtual void repaint(void) override;

	private:
		XYPlot *		m_plot;

		XYPlotAxis() = delete;
		XYPlotAxis(XYPlotAxis&) = delete;
		XYPlotAxis& operator = (XYPlotAxis&) = delete;
	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotWidgetCanvas : public QwtPlotCanvas {
		//Q_OBJECT
	public:
		XYPlotWidgetCanvas(XYPlot * _plot);
		virtual ~XYPlotWidgetCanvas();

	private:
		XYPlot *	m_plot;

	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotMarker : public QwtPlotMarker {
	public:
		XYPlotMarker(int _id);
		virtual ~XYPlotMarker();

		int id(void) const { return m_id; }

		void setStyle(const QColor & _innerColor, const QColor & _outerColor, int _size = 8, double _outerColorSize = 2.0);

	private:

		int			m_id;
		QwtSymbol *	m_symbol;

		XYPlotMarker() = delete;
		XYPlotMarker(XYPlotMarker&) = delete;
		XYPlotMarker& operator = (XYPlotMarker&) = delete;

	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotPanner : public QwtPlotPanner
	{
	public:
		XYPlotPanner(XYPlot * _plot);

		virtual bool eventFilter(QObject * _object, QEvent * _event) override;

	private:
		bool		m_mouseIsPressed;
	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotZoomer : public QwtPlotZoomer
	{
	public:
		XYPlotZoomer(XYPlot * _plot);

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
		XYPlot *		m_plot;
		bool			m_mouseMoved;
		bool			m_controlIsPressed;
	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotMagnifier : public QwtPlotMagnifier {
	public:
		XYPlotMagnifier(XYPlot * _plot);

		virtual void widgetMousePressEvent(QMouseEvent * _event) override;

		virtual void widgetMouseMoveEvent(QMouseEvent * _event) override;

		virtual void widgetMouseReleaseEvent(QMouseEvent * _event) override;

		virtual void widgetWheelEvent(QWheelEvent * _wheelEvent) override;

		virtual void rescale(double _factor) override;

		XYPlotMarker * marker(void) const { return m_marker; }

		void setAxisValueLabels(AbstractPlotAxis::AxisID _axis, const std::map<double, QString>& _valueToLabelMap);

		void clearAxisValueLabels(AbstractPlotAxis::AxisID _axis);

	private:
		QPoint			m_cursorPos;
		bool			m_rightMouseIsPressed;
		bool			m_mouseMoved;
		XYPlot *		m_plot;
		XYPlotMarker *	m_marker;

		std::map<double, QString>	m_valueLabelMapXBottom;
		std::map<double, QString>	m_valueLabelMapXTop;
		std::map<double, QString>	m_valueLabelMapYLeft;
		std::map<double, QString>	m_valueLabelMapYRight;

	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotLegend : public QwtLegend {
	public:
		explicit XYPlotLegend(XYPlot * _owner);
		virtual ~XYPlotLegend();

	private:
		XYPlot *	m_owner;

	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotGrid : public QwtPlotGrid {
	public:
		XYPlotGrid();
		virtual ~XYPlotGrid();

		void setColor(const QColor & _color, bool _repaint = true);

		void setWidth(double _width, bool _repaint = true);

	private:

		double		m_width;
		QColor		m_color;

	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT XYPlotPicker : public QwtPlotPicker {
	public:
		explicit XYPlotPicker(XYPlot * _plot);
		virtual ~XYPlotPicker();

	private:
		XYPlot *		m_plot;
	};
}