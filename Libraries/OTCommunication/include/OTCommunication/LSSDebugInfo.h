//! @file LSSDebugInfo.h
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot {

	class OT_COMMUNICATION_API_EXPORT LSSDebugInfo : public ot::Serializable {
		OT_DECL_DEFCOPY(LSSDebugInfo)
		OT_DECL_DEFMOVE(LSSDebugInfo)
	public:
		LSSDebugInfo() = default;
		virtual ~LSSDebugInfo() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	private:


	};
}