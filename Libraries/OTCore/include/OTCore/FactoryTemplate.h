//! @file FactoryTemplate.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"

// std header
#include <map>
#include <memory>
#include <functional>

namespace ot {

	//! @class FactoryTemplate
	//! @brief The FactoryTemplate may be used to quickly create default factories that are only used for item creation without any further logic.
	//! @note Note that the createFromJson() methods will call setFromJsonObject() if the created item inherits from Serializable.
	//! @tparam KeyType Key type (e.g. std::string).
	//! @tparam ValueType Value type (e.g. MyClass).
	template<class KeyType, class ValueType> class FactoryTemplate {
	public:
		//! @brief Constructor type definition.
		typedef std::function<ValueType*()> ConstructorFunctionType;

		//! @brief Will register the prvided constructor under the given key.
		//! If a constructor for the given key already exists a warning log will be generated and the request will be ignored.
		//! @param _key The constructor key.
		//! @param _constructor The constructor.
		void registerConstructor(const KeyType& _key, ConstructorFunctionType _constructor);

		//! @brief Remove the constructor for the given key.
		//! @param _key The constructor key.
		void deregisterConstructor(const KeyType& _key);

		//! @brief Call the constructor for the given key and return the created instance.
		//! @param _key The constructor key.
		ValueType* createFromKey(const KeyType& _key);

		//! @brief Creates an instance according to the key in the provided JSON object.
		//! If the key is empty the request will be ignored.
		//! If the created instance inherits from Serializable the setFromJsonObject method will be called.
		//! @param _jsonObject The JSON object containing the key and optionally additional information.
		//! @param _typeKey The JSON object key by which the constructor key may be retreived.
		ValueType* createFromJSON(const ConstJsonObject& _jsonObject, const char* _typeKey);

		//! @brief [/ref ValueType* createFromJSON(const ConstJsonObject& _jsonObject, const char* _typeKey) "x"].
		ValueType* createFromJSON(const ConstJsonObject& _jsonObject, const std::string& _typeKey);

	protected:
		//! @brief Protected constructor.
		FactoryTemplate() {};

		//! @brief Protected destructor.
		~FactoryTemplate() {};

	private:
		std::map<KeyType, ConstructorFunctionType> m_constructors; //! @brief Key to constructor map.
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @brief The FactoryRegistrarTemplate may be used to register a constructor at the factory.
	//! @note Note that the factory used to register at must have a public static instance method which returns a reference to the factory instance.
	//! @tparam KeyType The type of the key used to store and later acces the constructor.
	//! @tparam FactoryType The factory that is used to register.
	//! @tparam CreatedType The type of the created instance.
	template <typename KeyType, class FactoryType, typename CreatedType>
	class FactoryRegistrarTemplate {
		OT_DECL_NOCOPY(FactoryRegistrarTemplate)
		OT_DECL_NODEFAULT(FactoryRegistrarTemplate)
	public:
		//! @brief Constructor.
		//! @param _key The key for the constructor that is used to register at the factory.
		FactoryRegistrarTemplate(const KeyType& _key);
		
		//! @brief Destructor.
		//! When called the constructor will be deregistered from the factory.
		virtual ~FactoryRegistrarTemplate();

		//! @brief Get the constructor key.
		const KeyType& getKey(void) const { return m_key; };

	private:
		KeyType m_key; //! @brief Constructor key.
	};

}

#include "OTCore/FactoryTemplate.hpp"