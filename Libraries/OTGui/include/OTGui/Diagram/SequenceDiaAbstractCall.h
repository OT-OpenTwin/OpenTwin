// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"

namespace ot {

	class OT_GUI_API_EXPORT SequenceDiaAbstractCall : public Serializable {
		OT_DECL_NOCOPY(SequenceDiaAbstractCall)
		OT_DECL_NOMOVE(SequenceDiaAbstractCall)
	public:
		enum CallType {
			DirectCall,
			SelfCall
		};
		
		static std::string callTypeToString(CallType _type);
		static CallType stringToCallType(const std::string& _str);

		static std::string callTypeJsonKey() { return "CallType"; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		SequenceDiaAbstractCall() = default;
		virtual ~SequenceDiaAbstractCall() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		virtual CallType getCallType() const = 0;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText() const { return m_text; };

	private:
		std::string m_text;

	};

}