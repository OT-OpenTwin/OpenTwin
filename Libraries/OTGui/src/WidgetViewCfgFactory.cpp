//! @file WidgetViewCfgFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/WidgetViewCfg.h"
#include "OTGui/WidgetViewCfgFactory.h"


ot::WidgetViewCfg* ot::WidgetViewCfgFactory::createView(const std::string& _configurationKey) {
	auto it = WidgetViewCfgFactory::constructors().find(_configurationKey);
	if (it == WidgetViewCfgFactory::constructors().end()) {
		OT_LOG_EAS("Unknown view type \"" + _configurationKey + "\"");
		return nullptr;
	}
	else {
		return it->second();
	}
}

ot::WidgetViewCfg* ot::WidgetViewCfgFactory::createView(ConstJsonObject& _configurationObject) {
	WidgetViewCfg* newView = createView(json::getString(_configurationObject, OT_JSON_MEMBER_WidgetViewCfgType));
	if (newView) {
		try {
			newView->setFromJsonObject(_configurationObject);
		}
		catch (const std::exception& _e) {
			OT_LOG_E(_e.what());
			delete newView;
		}
		catch (...) {
			OT_LOG_E("Unknown error");
			delete newView;
		}
	}
	return newView;
}

void ot::WidgetViewCfgFactory::registerWidgetViewCfgConstructor(const std::string& _key, const WidgetViewCfgConstructor& _constructor) {
	if (WidgetViewCfgFactory::constructors().find(_key) != WidgetViewCfgFactory::constructors().end()) {
		OT_LOG_WAS("WidgetViewCfg constructor for key \"" + _key + "\" already exists. Ignoring...");
		return;
	}
	else {
		WidgetViewCfgFactory::constructors().insert_or_assign(_key, _constructor);
	}
}

std::map<std::string, ot::WidgetViewCfgConstructor>& ot::WidgetViewCfgFactory::constructors() {
	static std::map<std::string, WidgetViewCfgConstructor> g_constructors;
	return g_constructors;
}