// @otlicense

#pragma once

#include "OTCore/Owner.h"

namespace ot
{
	class __declspec(dllexport)  OwnerService : public ot::Owner<ot::serviceID_t>
	{
	public:
		OwnerService(ot::serviceID_t id = ot::invalidServiceID) : ot::Owner<ot::serviceID_t>(id) {};
		bool getIdFromJson(JsonValue& _object, ot::serviceID_t& _id);
		ot::OwnerService ownerFromJson(JsonValue& _object);
		void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator);

	private:
		const std::string m_jsonMemberName = "goid_";
	};
}
