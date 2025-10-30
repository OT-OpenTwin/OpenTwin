// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Plot.h"
#include "OTWidgets/WidgetView.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT PlotView : public WidgetView {
		OT_DECL_NOCOPY(PlotView)
		OT_DECL_NOMOVE(PlotView)
	public:
		PlotView();
		virtual ~PlotView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		Plot* getPlot(void) const { return m_plot; };

	private:
		Plot* m_plot;

	};

}
