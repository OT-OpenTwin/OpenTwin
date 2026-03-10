// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/Variable/Variable.h"

// std map
#include <map>

namespace ot {

	class OT_CORE_API_EXPORT VariableData : public Serializable
	{
		OT_DECL_DEFCOPY(VariableData)
		OT_DECL_DEFMOVE(VariableData)
	public:
		VariableData() = default;
		VariableData(const ConstJsonObject& _jsonObject);
		virtual ~VariableData() = default;

		void addVariable(const std::string& _name, const Variable& _variable);
		bool hasVariable(const std::string& _name) const;
		const Variable& getVariable(const std::string& _name) const;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

	private:
		std::map<std::string, Variable> m_variables;

	};

}