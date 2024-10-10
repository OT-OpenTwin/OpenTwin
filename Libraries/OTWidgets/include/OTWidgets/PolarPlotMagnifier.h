//! @file PolarPlotMagnifier.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_polar_magnifier.h>

namespace ot {

	class PolarPlot;
	class PolarPlotMarker;

	class OT_WIDGETS_API_EXPORT PolarPlotMagnifier : public QwtPolarMagnifier {
		OT_DECL_NOCOPY(PolarPlotMagnifier)
		OT_DECL_NODEFAULT(PolarPlotMagnifier)
	public:
		PolarPlotMagnifier(PolarPlot* _plot);
		virtual ~PolarPlotMagnifier();

		virtual void widgetMousePressEvent(QMouseEvent* _event) override;

		virtual void widgetMouseMoveEvent(QMouseEvent* _event) override;

		virtual void widgetMouseReleaseEvent(QMouseEvent* _event) override;

		virtual void widgetWheelEvent(QWheelEvent* _wheelEvent) override;

		virtual void rescale(double _factor) override;


	private:
		PolarPlot* m_plot;
		bool				m_rightMouseIsPressed;
		bool				m_mouseMoved;
		PolarPlotMarker* m_marker;
	};

}