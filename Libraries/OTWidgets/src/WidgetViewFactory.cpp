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
		if (!newView->setupViewFromConfig(_viewConfiguration)) {
			OT_LOG_W("Failed to setup view. Cleaning up...");
			delete newView;
			newView = nullptr;
		}
	}
	return newView;
}

ot::WidgetView* ot::WidgetViewFactory::createView(const std::string& _viewType) {
	auto it = WidgetViewFactory::constructors().find(_viewType);
	if (it == WidgetViewFactory::constructors().end()) {
		OT_LOG_EAS("Unknown view type \"" + _viewType + "\"");
		return nullptr;
	}
	else {
		return it->second();
	}
}

void ot::WidgetViewFactory::registerWidgetViewConstructor(const std::string& _key, const WidgetViewConstructor& _constructor) {
	if (WidgetViewFactory::constructors().find(_key) != WidgetViewFactory::constructors().end()) {
		OT_LOG_WAS("WidgetView constructor for key \"" + _key + "\" already exists. Ignoring...");
		return;
	}
	else {
		WidgetViewFactory::constructors().insert_or_assign(_key, _constructor);
	}
}

std::map<std::string, ot::WidgetViewConstructor>& ot::WidgetViewFactory::constructors() {
	static std::map<std::string, WidgetViewConstructor> g_constructors;
	return g_constructors;
}