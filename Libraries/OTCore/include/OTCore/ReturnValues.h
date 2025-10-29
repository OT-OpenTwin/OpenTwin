// @otlicense

#pragma once
#include <map>

#include "OTCore/Variable.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/Serializable.h"

namespace ot
{
	class OT_CORE_API_EXPORT ReturnValues : public ot::Serializable
	{
	public:
		ReturnValues() {}
		~ReturnValues();

		//! @brief Deep copy of GenericDataStructs
		ReturnValues(const ReturnValues& other);
		ReturnValues(ReturnValues&& other) noexcept = default;

				
		//! @brief Deep copy of GenericDataStructs
		ReturnValues& operator=(const ReturnValues& other);
		ReturnValues& operator=(ReturnValues&& other) noexcept = default;

		bool operator==(const ReturnValues& other) const;
		bool operator!=(const ReturnValues& other) const;
		
		//! @brief Does not take over ownership. Document needs to stay alive until ReturnValues object gets serialised as part of a ReturnMessage
		void addData(const std::string& _entryName, const ot::JsonValue* _values);
		uint64_t getNumberOfEntries() { return m_valuesByName.size(); }
		
		//! @brief 
		//! @return Empty document if ReturnValues was not created by deserialisation of json object.
		ot::JsonDocument& getValues() { return m_valuesAsDoc; };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	private:
		std::map<std::string, const ot::JsonValue*> m_valuesByName;
		ot::JsonDocument m_valuesAsDoc;
	};
}