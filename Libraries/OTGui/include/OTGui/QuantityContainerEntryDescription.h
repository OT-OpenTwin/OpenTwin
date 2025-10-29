// @otlicense

#pragma once
#pragma warning(disable:4251)

#include "OTCore/Serializable.h"
#include <string>

namespace ot
{
	struct __declspec(dllexport) QuantityContainerEntryDescription : public ot::Serializable
	{
		//This one is also bson serialised in the EntityResult1DCurve entity. Any changes here in the config also need to be done in the 
		std::string m_fieldName = "";
		std::string m_label = "";
		std::string m_unit = "";
		std::string m_dataType = "";
		std::vector<uint32_t> m_dimension;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};
}