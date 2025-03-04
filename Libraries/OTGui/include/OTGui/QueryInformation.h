#pragma once
#include <string>
#include "OTCore/Serializable.h"

namespace ot
{
	struct __declspec(dllexport) QueryInformation : public ot::Serializable
	{
		std::string m_query;
		std::string m_projection;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};
}
