//! @file WidgetViewCfgRegistrar.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <string>

namespace ot {

	//! @class WidgetViewCfgRegistrar
	//! @brief The WidgetViewCfgRegistrar can be used to register a /ref WidgetViewCfg "WidgetViewCfg" at the /ref WidgetViewCfgFactory "WidgetViewCfgFactory".
	template <class T> class WidgetViewCfgRegistrar {
	public:
		//! @brief Assignment constructor.
		//! @param _key The used for the registration at the /ref WidgetViewCfgFactory "factory".
		WidgetViewCfgRegistrar(const std::string& _key);
		~WidgetViewCfgRegistrar() {};
	private:
		//! @brief Deleted private constructor.
		WidgetViewCfgRegistrar() = delete;

		//! @brief Deleted copy constructor.
		WidgetViewCfgRegistrar(const WidgetViewCfgRegistrar&) = delete;

		//! @brief Deleted assignment operator.
		WidgetViewCfgRegistrar& operator = (const WidgetViewCfgRegistrar&) = delete;
	};

}

#include "OTGui/WidgetViewCfgRegistrar.hpp"