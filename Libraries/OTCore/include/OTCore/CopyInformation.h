//! @file CopyInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_CORE_API_EXPORT CopyInformation : public Serializable {
	public:
		static std::string getCopyTypeJsonKey(void);
		static std::string getCopyVersionJsonKey(void);

		CopyInformation() = default;
		CopyInformation(const CopyInformation&) = default;
		CopyInformation(CopyInformation&&) = default;
		virtual ~CopyInformation() = default;

		CopyInformation& operator = (const CopyInformation&) = default;
		CopyInformation& operator = (CopyInformation&&) = default;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		virtual std::string getCopyType(void) const = 0;
		virtual int getCopyVersion(void) const = 0;

	};

}