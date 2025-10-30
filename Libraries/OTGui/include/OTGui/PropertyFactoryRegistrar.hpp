// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/PropertyFactory.h"
#include "OTGui/PropertyFactoryRegistrar.h"

template <class T>
ot::PropertyFactoryRegistrar<T>::PropertyFactoryRegistrar(const std::string& _key) {
	PropertyFactory::registerProperty(_key, []() { return new T; });
}
