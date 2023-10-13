#pragma once
#include "OpenTwinCore/Owner.h"

namespace ot
{
	class __declspec(dllexport)  OwnerService : public ot::Owner<ot::serviceID_t>
	{
	public:
		OwnerService(ot::serviceID_t id = ot::invalidServiceID) : ot::Owner<ot::serviceID_t>(id) {};
		bool getIdFromJson(OT_rJSON_val& _object, ot::serviceID_t& _id);
		ot::OwnerService ownerFromJson(OT_rJSON_val& _object);
		void addToJsonObject(OT_rJSON_doc& _doc, OT_rJSON_val& _object);

	private:
		const std::string m_jsonMemberName = "goid_";
	};
}
