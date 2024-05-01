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

	//! @brief Constructor type for WidgetViewCfg creation.
	using WidgetViewCfgConstructor = std::function<WidgetViewCfg* ()>;

	//! @class WidgetViewCfgFactory
	//! @brief Factory for creating /ref WidgetViewCfg "WidgetViewCfg" instances.
	//! The widget view configuration factory provides static methods for creating widget view oconfigurations.
	class OT_GUI_API_EXPORT WidgetViewCfgFactory {
		OT_DECL_NOCOPY(WidgetViewCfgFactory)
	public:
		//! @brief Create a not configured widget view configuration.
		//! @param _configurationKey The key to the WidgetViewCfgConstructor.
		static WidgetViewCfg* createView(const std::string& _configurationKey);

		//! @brief Create and configure a widget view.
		//! @param _configurationObject Json object containing the widget view configuration.
		static WidgetViewCfg* createView(ConstJsonObject& _configurationObject);

		//! @brief Register a new widget view cofiguration constructor.
		//! If a constructor for the given name already exists a warning log will be gerated and the request method will return.
		//! @param _key The unique key of the WidgetViewCfgConstructor
		//! @param _constructor Constructor used to create the widget view.
		static void registerWidgetViewCfgConstructor(const std::string& _key, const WidgetViewCfgConstructor& _constructor);

	private:
		//! @brief Returns a reference to the constructors map.
		static std::map<std::string, WidgetViewCfgConstructor>& constructors();

		//! @brief Private constructor.
		WidgetViewCfgFactory() {};

		//! @brief Private destructor.
		~WidgetViewCfgFactory() {};
	};

}