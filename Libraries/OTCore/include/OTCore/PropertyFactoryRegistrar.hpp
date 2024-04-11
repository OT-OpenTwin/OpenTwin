//! @file PropertyFactoryRegistrar.hpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/PropertyFactory.h"
#include "OTCore/PropertyFactoryRegistrar.h"

template <class T>
ot::PropertyFactoryRegistrar<T>::PropertyFactoryRegistrar(const std::string& _key) {
	PropertyFactory::registerProperty(_key, []() { return new T; });
}
