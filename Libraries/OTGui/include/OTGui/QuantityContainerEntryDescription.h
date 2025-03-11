#pragma once
#pragma warning(disable:4251)

#include "OTCore/Serializable.h"
#include <string>

namespace ot
{
	struct __declspec(dllexport) QuantityContainerEntryDescription : public ot::Serializable
	{
		std::string m_fieldName = "";
		std::string m_label = "";
		std::string m_unit = "";
		std::string m_dataType = "";

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};
}