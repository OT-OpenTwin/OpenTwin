/*
 *	File:		PolarPlot.h
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
#include <qwt_polar_plot.h>
#include <qwt_polar_grid.h>
#include <qwt_polar_panner.h>
#include <qwt_polar_marker.h>
#include <qwt_polar_magnifier.h>
#include <qwt_legend.h>

namespace qwtw {
	class Plot;
	class PolarPlotAxis;
	class PolarPlotGrid;
	class PolarPlotLegend;
	class PolarPlotPanner;
	class PolarPlotMagnifier;
	class PolarPlotMarker;

	class QWTW_LIB_EXPORT PolarPlot : public QwtPolarPlot, public AbstractPlot {
	public:
		PolarPlot(Plot * _owner);
		virtual ~PolarPlot();

		// ################################################################

		// Plot

		virtual void RepaintLegend(void) override;

		virtual void RefreshWholePlot(void) override;

		virtual void Clear(void) override;

		void setAxisValueLabels(AbstractPlotAxis::AxisID _axis, const std::map<double, QString>& _valueToLabelMap);

		void clearAxisValueLabels(AbstractPlotAxis::AxisID _axis);

		// ################################################################

		// Grid

		virtual void RepaintGrid(void) override;

		// ################################################################

	private:
		Plot *		m_owner;

		PolarPlotGrid *			m_grid;
		PolarPlotLegend *		m_legend;
		PolarPlotMagnifier *	m_magnifier;
		PolarPlotPanner *		m_panner;

		PolarPlot() = delete;
		PolarPlot(PolarPlot&) = delete;
		PolarPlot& operator = (PolarPlot&) = delete;

	};

	// #####################################################################################################

	// #####################################################################################################

	// #####################################################################################################

	class QWTW_LIB_EXPORT PolarPlotAxis : public AbstractPlotAxis {
	public:
		PolarPlotAxis(AxisID _axisID, PolarPlot * _plot);
		virtual ~PolarPlotAxis();

		virtual void repaint(void) override;

	private:
		PolarPlot *		m_plot;

		PolarPlotAxis() = delete;
		PolarPlotAxis(PolarPlotAxis&) = delete;
		PolarPlotAxis& operator = (PolarPlotAxis&) = delete;
	};

	// ###############################################################################

	// ###############################################################################

	// ###############################################################################

	class QWTW_LIB_EXPORT PolarPlotGrid : public QwtPolarGrid {
	public:
		PolarPlotGrid(PolarPlot * _plot);
		virtual ~PolarPlotGrid();

	private:
		PolarPlot *			m_plot;

		PolarPlotGrid() = delete;
		PolarPlotGrid(PolarPlotGrid&) = delete;
		PolarPlotGrid& operator = (PolarPlotGrid&) = delete;
	};

	// ###############################################################################

	// ###############################################################################

	// ###############################################################################

	class QWTW_LIB_EXPORT PolarPlotLegend : public QwtLegend {
	public:
		PolarPlotLegend(PolarPlot * _plot);
		virtual ~PolarPlotLegend();

	private:
		PolarPlot *		m_plot;
	};

	// ###############################################################################

	// ###############################################################################

	// ###############################################################################

	class QWTW_LIB_EXPORT PolarPlotPanner : public QwtPolarPanner {
	public:
		PolarPlotPanner(PolarPlot * _plot);
		virtual ~PolarPlotPanner();

	private:
		PolarPlot *		m_plot;
	};

	// ###############################################################################

	// ###############################################################################

	// ###############################################################################

	class QWTW_LIB_EXPORT PolarPlotMagnifier : public QwtPolarMagnifier {
	public:
		PolarPlotMagnifier(PolarPlot * _plot);
		virtual ~PolarPlotMagnifier();

		virtual void widgetMousePressEvent(QMouseEvent * _event) override;

		virtual void widgetMouseMoveEvent(QMouseEvent * _event) override;

		virtual void widgetMouseReleaseEvent(QMouseEvent * _event) override;

		virtual void widgetWheelEvent(QWheelEvent * _wheelEvent) override;

		virtual void rescale(double _factor) override;


	private:
		PolarPlot *			m_plot;
		QwtPolarCanvas *	m_canvas;
		bool				m_rightMouseIsPressed;
		bool				m_mouseMoved;
		PolarPlotMarker *	m_marker;
	};

	// ###############################################################################

	// ###############################################################################

	// ###############################################################################

	class QWTW_LIB_EXPORT PolarPlotMarker : public QwtPolarMarker {
	public:
		PolarPlotMarker(PolarPlot * _plot);
		virtual ~PolarPlotMarker();

	private:
		PolarPlot *			m_plot;
	};
}