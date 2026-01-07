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

	private:
		std::string m_text;

	};

}