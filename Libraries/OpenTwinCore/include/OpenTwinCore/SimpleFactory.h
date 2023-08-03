//! @file SimpleFactory.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/Singleton.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"

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
#define OT_RegisterSimpleFactoryObject(___class, ___key) static ot::SimpleFactoryRegistrar<___class> SimpleFactoryRegistrar(___key)
//#define OT_RegisterSimpleFactoryObject(___class, ___key) static ot::SimpleFactoryRegistrar<___class> ___class##SimpleFactoryRegistrar(___key)

//! @brief Key used in a json object to determine the simple factory object key
#define OT_SimpleFactoryJsonKey "OTSimpleFactoryObject.Key"

#pragma warning(disable:4251)

namespace ot {

	//! @brief Base class for any class that may be created by the simple factory
	//! Note that the OT_SimpleFactoryObject macro should be used to register the class in the factory
	class OT_CORE_API_EXPORT SimpleFactoryObject {
	public:
		SimpleFactoryObject() = default;
		SimpleFactoryObject(const SimpleFactoryObject&) = default;
		virtual ~SimpleFactoryObject() {};

		SimpleFactoryObject& operator = (const SimpleFactoryObject&) = default;

		void addSimpleFactoryObjectNameToJson(OT_rJSON_doc& _doc, OT_rJSON_val& _jsonObj) const;

		void setSimpleFactoryObjectName(const std::string& _name) { m_sfoname = _name; };
		const std::string& simpleFactoryObjectName(void) const { return m_sfoname; };

	private:
		std::string m_sfoname;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class SimpleFactory;

	template <class T>
	class OT_CORE_API_EXPORTONLY SimpleFactoryRegistrar {
	public:
		SimpleFactoryRegistrar(std::string _name) {
			ot::SimpleFactory::instance().add(_name, [](void)->SimpleFactoryObject* {return new T(); });
		}
		virtual ~SimpleFactoryRegistrar() {};
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################
	
	class OT_CORE_API_EXPORT SimpleFactory : public ot::Singleton<SimpleFactory> {
		OT_SINGLETON(SimpleFactory)
	public:

		//! @brief Create an object according to the provided key
		//! @param _key The key to the object
		SimpleFactoryObject* create(const std::string& _key);

		//! @brief Register the provided constructor under the given key
		//! @param _key The key
		//! @param _classConstructor Constructor
		void add(const std::string& _key, std::function<SimpleFactoryObject* (void)> _classConstructor);

		//! @brief Removes the registered key
		void remove(const std::string& _key);

		//! @brief Returns true if the provided key was registered before
		//! @param _key Key to check
		bool contains(const std::string& _key);

		//! @brief Create an object according to the provided key and ensure its type
		//! If the created object can not be dynamically casted to the specified type the created object will be destroyed and nullptr will be returned
		//! @param _key The key to the object
		template<class T> T* createType(const std::string& _key);

		//! @brief Create an object according to the key in the provided json object and ensure its type
		//! If the created object can not be dynamically casted to the specified type the created object will be destroyed and nullptr will be returned
		//! @param _key The key to the object
		template<class T> T* createType(OT_rJSON_val& _jsonObject);

	private:
		std::map<std::string, std::function<SimpleFactoryObject* (void)>> m_data;

		SimpleFactory() {};
		virtual ~SimpleFactory() {};

		SimpleFactory(const SimpleFactory&) = delete;
		SimpleFactory& operator = (const SimpleFactory&) = delete;
	};

}

template <class T>
T* ot::SimpleFactory::createType(const std::string& _key) {
	SimpleFactoryObject* obj = create(_key);
	if (obj == nullptr) return nullptr;
	T* c = dynamic_cast<T*>(obj);
	if (c == nullptr) delete obj;
	return c;
}

template <class T>
T* ot::SimpleFactory::createType(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, OT_SimpleFactoryJsonKey, String);
	return ot::SimpleFactory::createType<T>(_jsonObject[OT_SimpleFactoryJsonKey].GetString());
}
