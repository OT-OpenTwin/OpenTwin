// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

namespace ot {

	class PlotManager;

	class __declspec(dllexport) PlotManagerView : public WidgetView {
	public:
		PlotManagerView();
		virtual ~PlotManagerView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		PlotManager* getPlotManager(void) const { return m_plotManager; };

	private:
		PlotManager* m_plotManager;

	};

}
