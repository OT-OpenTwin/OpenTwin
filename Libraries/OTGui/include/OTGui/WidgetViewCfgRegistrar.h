//! @file WidgetViewCfgRegistrar.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <string>

namespace ot {

	template <class T>
	class WidgetViewCfgRegistrar {
	public:
		WidgetViewCfgRegistrar(const std::string& _key);
		~WidgetViewCfgRegistrar() {};
	private:
		WidgetViewCfgRegistrar() = delete;
		WidgetViewCfgRegistrar(const WidgetViewCfgRegistrar&) = delete;
		WidgetViewCfgRegistrar& operator = (const WidgetViewCfgRegistrar&) = delete;
	};

}

#include "OTGui/WidgetViewCfgRegistrar.hpp"