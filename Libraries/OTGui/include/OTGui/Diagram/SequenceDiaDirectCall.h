// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Diagram/SequenceDiaAbstractCall.h"

namespace ot {

	class OT_GUI_API_EXPORT SequenceDiaDirectCall : public SequenceDiaAbstractCall {
		OT_DECL_NOCOPY(SequenceDiaDirectCall)
		OT_DECL_NOMOVE(SequenceDiaDirectCall)
	public:
		SequenceDiaDirectCall() = default;
		virtual ~SequenceDiaDirectCall() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		virtual CallType getCallType() const override { return CallType::DirectCall; };

		void setFunctionName(const std::string& _name) { m_functionName = _name; };
		const std::string& getFunctionName() const { return m_functionName; };

	private:
		std::string m_functionName;

	};

}