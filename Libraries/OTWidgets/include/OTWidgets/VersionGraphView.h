//! @file VersionGraphView.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/VersionGraph.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT VersionGraphView : public VersionGraph, public WidgetView {
	public:
		VersionGraphView();
		virtual ~VersionGraphView();

		virtual QWidget* getViewWidget(void) override { return this; };

	private:

	};

}