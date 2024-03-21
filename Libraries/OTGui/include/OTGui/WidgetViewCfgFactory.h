//! @file WidgetViewCfgFactory.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <map>
#include <string>
#include <functional>

namespace ot {

	class WidgetViewCfg;

	using WidgetViewCfgConstructor = std::function<WidgetViewCfg* ()>;

	class OT_GUI_API_EXPORT WidgetViewCfgFactory {
		OT_DECL_NOCOPY(WidgetViewCfgFactory)
	public:
		static WidgetViewCfg* createView(const std::string& _configurationKey);

		static WidgetViewCfg* createView(ConstJsonObject& _configurationObject);

		static void registerWidgetViewCfgConstructor(const std::string& _key, const WidgetViewCfgConstructor& _constructor);

	private:
		static std::map<std::string, WidgetViewCfgConstructor>& constructors();

		WidgetViewCfgFactory() {};
		~WidgetViewCfgFactory() {};
	};

}