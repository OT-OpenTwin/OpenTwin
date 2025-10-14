//! @file EntityFactoryRegistrar.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "EntityFactory.h"

// std header
#include <type_traits>

//! @brief The EntityFactoryRegistrar class is a helper class that allows to register entity classes with the EntityFactory.
//! @tparam T The class to register. Must inherit from EntityBase.
template<typename T>
class EntityFactoryRegistrar {
public:

	//! @brief Constructor that registers the class with the factory using the default constructor of the class.
	//! @param _className The name of the class to register.
    EntityFactoryRegistrar(const std::string& _className)
    {
		static_assert(std::is_base_of<EntityBase, T>::value, "T must inherit from EntityBase");
		static_assert(std::is_default_constructible<T>::value, "T must be default constructible if no create function is provided");
		EntityFactory::instance().registerClass(_className, []() -> EntityBase* { return new T(); });
	}

	//! @brief Constructor that registers the class with the factory using a custom create function.
	//! @param _className The name of the class to register.
	//! @param _createFunction A function that creates an instance of the class.
    EntityFactoryRegistrar(const std::string& _className, EntityFactory::CreateEntityFunction _createFunction) {
        static_assert(std::is_base_of<EntityBase, T>::value, "T must inherit from EntityBase");
		EntityFactory::instance().registerClass(_className, std::forward(_createFunction));
    }

};