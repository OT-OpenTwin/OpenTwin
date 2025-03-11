#pragma once
#include <string>
#include "OTCore/Serializable.h"
#include "QuantityContainerEntryDescription.h"

#pragma warning(disable:4251)

namespace ot
{
	struct __declspec(dllexport) QueryInformation : public ot::Serializable
	{
		std::string m_query;
		std::string m_projection;

		QuantityContainerEntryDescription m_quantityDescription;
		
		//Curves are using the parameter prioritised by their order in this list. E.g. the first entry is used in a 1D plot as X-Axis, the remaining are used for plotting the curve family
		std::list<QuantityContainerEntryDescription> m_parameterDescriptions; 

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};	
}
