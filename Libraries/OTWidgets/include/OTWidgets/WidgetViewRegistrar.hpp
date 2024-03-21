//! @file WidgetViewRegistrar.hpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetViewFactory.h"
#include "OTWidgets/WidgetViewRegistrar.h"

template <class T>
ot::WidgetViewRegistrar<T>::WidgetViewRegistrar(const std::string& _key) {
	WidgetViewFactory::registerWidgetViewConstructor(_key, []() { return new T; });
}
