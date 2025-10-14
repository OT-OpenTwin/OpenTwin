//! @file EntityAPI.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

namespace ot {

	class __declspec(dllexport) EntityAPI {
		OT_DECL_NOCOPY(EntityAPI)
		OT_DECL_NOMOVE(EntityAPI)
		OT_DECL_NODEFAULT(EntityAPI)
	public:
		static EntityBase* readEntityFromEntityIDandVersion(UID _entityID, UID _version);

	private:

	};

}