//! @file WidgetViewFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/WidgetViewCfg.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/WidgetViewFactory.h"

ot::WidgetView* ot::WidgetViewFactory::createView(WidgetViewCfg* _viewConfiguration) {
	OTAssertNullptr(_viewConfiguration);
	ot::WidgetView* newView = WidgetViewFactory::createView(_viewConfiguration->getViewType());
	if (newView) {
		if (!newView->setupFromConfig(_viewConfiguration)) {
			OT_LOG_W("Failed to setup view. Cleaning up...");
			delete newView;
			newView = nullptr;
		}
	}
	return newView;
}

ot::WidgetView* ot::WidgetViewFactory::createView(const std::string& _viewType) {
	if (_viewType == "") {

	}
	else {
		OT_LOG_EAS("Unknown view type \"" + _viewType + "\"");
	}
	return nullptr;
}