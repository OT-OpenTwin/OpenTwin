// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

namespace ot {
	
	class OT_SERVICEFOUNDATION_API_EXPORTONLY AbstractModelNotifier {
		OT_DECL_NOCOPY(AbstractModelNotifier)
		OT_DECL_NOMOVE(AbstractModelNotifier)
	public:
		AbstractModelNotifier() = default;
		virtual ~AbstractModelNotifier() = default;
	};

}