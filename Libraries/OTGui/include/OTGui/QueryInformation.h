#pragma once
#include <string>
#include "OTCore/Serializable.h"

#pragma warning(disable:4251)

namespace ot
{
	struct __declspec(dllexport) QueryInformation : public ot::Serializable
	{
		std::string m_query;
		std::string m_projection;

		std::string m_quantityFieldName;
		std::string m_parameterFieldName;
		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};
}
