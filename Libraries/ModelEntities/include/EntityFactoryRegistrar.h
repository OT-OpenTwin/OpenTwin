// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/FileExtension.h"
#include "EntityFactory.h"

// std header
#include <type_traits>

//! @brief The EntityFactoryRegistrar class is a helper class that allows to register entity classes with the EntityFactory.
//! @tparam T The class to register. Must inherit from EntityBase.
template<typename T>
class EntityFactoryRegistrar {
public:

	//! @brief Constructor that registers the class with the factory using the default constructor of the class.
	//! @param _fileExtension The file extension to register the class for.
	EntityFactoryRegistrar(ot::FileExtension::DefaultFileExtension _fileExtension) {
		static_assert(std::is_base_of<EntityBase, T>::value, "T must inherit from EntityBase");
		static_assert(std::is_default_constructible<T>::value, "T must be default constructible if no create function is provided");
		EntityFactory::instance().registerClass(_fileExtension, []() -> EntityBase* { return new T(); });
	}

	//! @brief Constructor that registers the class with the factory using a custom create function.
	//! @param _fileExtension The file extension to register the class for.
	//! @param _createFunction A function that creates an instance of the class.
	EntityFactoryRegistrar(ot::FileExtension::DefaultFileExtension _fileExtension, EntityFactory::CreateEntityFunction _createFunction) {
		static_assert(std::is_base_of<EntityBase, T>::value, "T must inherit from EntityBase");
		EntityFactory::instance().registerClass(_fileExtension, std::forward(_createFunction));
	}

	//! @brief Constructor that registers the class with the factory using the default constructor of the class.
	//! @param _fileExtension The file extension to register the class for.
	EntityFactoryRegistrar(std::initializer_list<ot::FileExtension::DefaultFileExtension> _fileExtensions) {
		static_assert(std::is_base_of<EntityBase, T>::value, "T must inherit from EntityBase");
		static_assert(std::is_default_constructible<T>::value, "T must be default constructible if no create function is provided");
		for (auto it = _fileExtensions.begin(); it != _fileExtensions.end(); it++) {
			EntityFactory::instance().registerClass(*it, []() -> EntityBase* { return new T(); });
		}
	}

	//! @brief Constructor that registers the class with the factory using a custom create function.
	//! @param _fileExtension The file extension to register the class for.
	//! @param _createFunction A function that creates an instance of the class.
	EntityFactoryRegistrar(std::initializer_list<ot::FileExtension::DefaultFileExtension> _fileExtensions, EntityFactory::CreateEntityFunction _createFunction) {
		static_assert(std::is_base_of<EntityBase, T>::value, "T must inherit from EntityBase");
		for (auto it = _fileExtensions.begin(); it != _fileExtensions.end(); it++) {
			EntityFactory::instance().registerClass(*it, std::forward(_createFunction));
		}
	}

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