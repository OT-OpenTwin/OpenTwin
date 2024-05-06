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
	//! @tparam KeyType Key type (e.g. std::string)
	//! @tparam ValueType Value type (e.g. MyClass)
	template<class KeyType, class ValueType> class FactoryTemplate {
	public:
		typedef std::function<ValueType*()> ConstructorFunctionType;

		void registerConstructor(const KeyType& _key, ConstructorFunctionType _constructor);

		void deregisterConstructor(const KeyType& _key);

		ValueType* createFromKey(const KeyType& _key);

		ValueType* createFromJSON(const ConstJsonObject& _jsonObject, const char* _typeKey);
		ValueType* createFromJSON(const ConstJsonObject& _jsonObject, const std::string& _typeKey);

	protected:
		FactoryTemplate() {};
		~FactoryTemplate() {};

	private:
		std::map<KeyType, ConstructorFunctionType> m_constructors;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	template <typename KeyType, class FactoryType, typename CreatedType>
	class FactoryRegistrarTemplate {
		OT_DECL_NOCOPY(FactoryRegistrarTemplate)
		OT_DECL_NODEFAULT(FactoryRegistrarTemplate)
	public:
		FactoryRegistrarTemplate(const KeyType& _key);
		virtual ~FactoryRegistrarTemplate();

		const KeyType& getKey(void) const { return m_key; };

	private:
		KeyType m_key;
	};

}

#include "OTCore/FactoryTemplate.hpp"