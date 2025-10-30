// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyInputFactory.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

template <class T>
ot::PropertyInputFactoryRegistrar<T>::PropertyInputFactoryRegistrar(const std::string& _key) {
	PropertyInputFactory::registerPropertyInput(_key, []() { return new T; });
}
