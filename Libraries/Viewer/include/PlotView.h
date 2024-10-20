//! @file PlotView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

#include "Plot.h"

#define OT_WIDGETTYPE_Plot "Plot"

namespace ot {

	class PlotView : public Plot, public WidgetView {
	public:
		PlotView(Viewer* _viewer = (Viewer*)nullptr);
		virtual ~PlotView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		virtual bool setupViewFromConfig(WidgetViewCfg* _config) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private:

	};

}