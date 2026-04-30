// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/ModelEntitiesAPIExport.h"

namespace ot {

	class OT_MODELENTITIES_API_EXPORT ModelEntitiesAPI {
	public:
		ModelEntitiesAPI() = delete;

		static void initialize();
	};

}