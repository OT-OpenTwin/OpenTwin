//! @file CartesianPlotMagnifier.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_magnifier.h>

// Qt header
#include <QtCore/qpoint.h>

namespace ot {

	class CartesianPlot;
	class CartesianPlotMarker;

	class OT_WIDGETS_API_EXPORT CartesianPlotMagnifier : public QwtPlotMagnifier {
	public:
		CartesianPlotMagnifier(CartesianPlot* _plot);

		virtual void widgetMousePressEvent(QMouseEvent* _event) override;

		virtual void widgetMouseMoveEvent(QMouseEvent* _event) override;

		virtual void widgetMouseReleaseEvent(QMouseEvent* _event) override;

		virtual void widgetWheelEvent(QWheelEvent* _wheelEvent) override;

		virtual void rescale(double _factor) override;

		CartesianPlotMarker* marker(void) const { return m_marker; }

	private:
		QPointF			m_cursorPos;
		bool			m_rightMouseIsPressed;
		bool			m_mouseMoved;
		CartesianPlot* m_plot;
		CartesianPlotMarker* m_marker;

	};

}