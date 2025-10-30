// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "EntityBase.h"

namespace ot {

	class OT_MODELENTITIES_API_EXPORT EntityAPI {
		OT_DECL_NOCOPY(EntityAPI)
		OT_DECL_NOMOVE(EntityAPI)
		OT_DECL_NODEFAULT(EntityAPI)
	public:
		static EntityBase* readEntityFromEntityIDandVersion(const ot::BasicEntityInformation& _entityInfo) { return readEntityFromEntityIDandVersion(_entityInfo.getEntityID(), _entityInfo.getEntityVersion()); };
		static EntityBase* readEntityFromEntityIDandVersion(const EntityBase& _entityInfo) { return readEntityFromEntityIDandVersion(_entityInfo.getEntityID(), _entityInfo.getEntityStorageVersion()); };
		static EntityBase* readEntityFromEntityIDandVersion(UID _entityID, UID _version);

	private:

	};

}