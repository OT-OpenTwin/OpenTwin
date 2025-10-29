// @otlicense

//! @file AbstractModelNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date March 2021
// ###########################################################################################################################################################################################################################################################################################################################

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