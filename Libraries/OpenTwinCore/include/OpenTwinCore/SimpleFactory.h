//! @file SimpleFactory.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/Singleton.h"

// std header
#include <string>
#include <map>
#include <functional>

//! @brief Add the specified object to the simple factory
//! Note that the class name should be provided without a namespace
//! Example:
//! namespace ot {
//!     OT_RegisterSimpleFactoryObject(foo);
//! }
//! 
//! Instead of:
//! OT_RegisterSimpleFactoryObject(ot::foo);
#define OT_RegisterSimpleFactoryObject(___className) static ot::SimpleFactoryRegistrar<___className> ___className##SimpleFactoryRegistrar(#___className)

#pragma warning(disable:4251)

namespace ot {

	//! @brief Base class for any class that may be created by the simple factory
	class OT_CORE_API_EXPORTONLY SimpleFactoryObject {
	public:
		SimpleFactoryObject() = default;
		SimpleFactoryObject(const SimpleFactoryObject&) = default;
		virtual ~SimpleFactoryObject() {};

		SimpleFactoryObject& operator = (const SimpleFactoryObject&) = default;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	template <class T>
	class OT_CORE_API_EXPORTONLY SimpleFactoryRegistrar {
	public:
		SimpleFactoryRegistrar(const std::string& _name);

	private:

		SimpleFactoryRegistrar() = delete;
		SimpleFactoryRegistrar(const SimpleFactoryRegistrar&) = delete;
		SimpleFactoryRegistrar& operator = (const SimpleFactoryRegistrar&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_CORE_API_EXPORT SimpleFactory : public ot::Singleton<SimpleFactory> {
		OT_SINGLETON(SimpleFactory)
	public:
		SimpleFactoryObject* create(const std::string& _key);
		void add(const std::string& _key, std::function<SimpleFactoryObject* (void)> _classConstructor);
		void remove(const std::string& _key);
		bool contains(const std::string& _key);

	private:
		std::map<std::string, std::function<SimpleFactoryObject* (void)>> m_data;

		SimpleFactory() {};
		virtual ~SimpleFactory() {};

		SimpleFactory(const SimpleFactory&) = delete;
		SimpleFactory& operator = (const SimpleFactory&) = delete;
	};

}

template <class T>
ot::SimpleFactoryRegistrar<T>::SimpleFactoryRegistrar(const std::string& _name) {
	ot::SimpleFactory::instance().add(_name, [](void)->SimpleFactoryObject* {return new T(); });
}