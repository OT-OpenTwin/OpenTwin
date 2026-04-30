// @otlicense

#pragma once

// OpenTwin header
#include "OTBlockEntities/BlockEntitiesAPIExport.h"

namespace ot {

	class OT_BLOCKENTITIES_API_EXPORT BlockEntitiesAPI {
	public:
		BlockEntitiesAPI() = delete;
		
		static void initialize();
	};
}