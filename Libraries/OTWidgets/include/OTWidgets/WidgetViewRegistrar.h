//! @file WidgetViewRegistrar.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <string>

namespace ot {

	//! @brief Used to register a WidgetView at the WidgetViewFactory
	template <class T>
	class WidgetViewRegistrar {
	public:
		WidgetViewRegistrar(const std::string& _key);
		~WidgetViewRegistrar() {};

	private:
		WidgetViewRegistrar() = delete;
		WidgetViewRegistrar(const WidgetViewRegistrar&) = delete;
		WidgetViewRegistrar& operator = (const WidgetViewRegistrar&) = delete;

	};

}

#include "OTWidgets/WidgetViewRegistrar.hpp"