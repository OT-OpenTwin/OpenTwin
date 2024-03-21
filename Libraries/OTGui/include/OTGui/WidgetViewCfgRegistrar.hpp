//! @file WidgetViewCfgRegistrar.hpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/WidgetViewCfgFactory.h"
#include "OTGui/WidgetViewCfgRegistrar.h"

template <class T>
ot::WidgetViewCfgRegistrar<T>::WidgetViewCfgRegistrar(const std::string& _key) {
	WidgetViewCfgFactory::registerWidgetViewCfgConstructor(_key, []() { return new T; });
}
