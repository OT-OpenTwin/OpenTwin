//! @file CartesianPlotZoomer.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_plot_zoomer.h>

namespace ot {

	class CartesianPlot;

	class OT_WIDGETS_API_EXPORT CartesianPlotZoomer : public QwtPlotZoomer {
	public:
		CartesianPlotZoomer(CartesianPlot* _plot);

		virtual void widgetMousePressEvent(QMouseEvent* _event) override;

		virtual void widgetMouseDoubleClickEvent(QMouseEvent* _event) override;

		virtual void widgetMouseMoveEvent(QMouseEvent* _event) override;

		virtual void widgetWheelEvent(QWheelEvent* _event) override;

		virtual void widgetMouseReleaseEvent(QMouseEvent* _event) override;

		virtual void widgetKeyPressEvent(QKeyEvent* _event) override;

		virtual void widgetKeyReleaseEvent(QKeyEvent* _event) override;

		bool isControlPressed(void) const { return m_controlIsPressed; }

	private:
		CartesianPlot* m_plot;
		bool           m_mouseMoved;
		bool           m_controlIsPressed;
	};

}