// @otlicense

#pragma once

// OpenTwin header
#include "OTCADEntities/CADModelEntitiesAPIExport.h"

namespace ot {

	class OT_CADENTITIES_API_EXPORT CADEntitiesAPI {
	public:
		CADEntitiesAPI() = delete;

		static void initialize();
	};

}