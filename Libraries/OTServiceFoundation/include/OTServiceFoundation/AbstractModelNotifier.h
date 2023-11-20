#pragma once

// Foundation header
#include "OTServiceFoundation/FoundationAPIExport.h"

// C++ header
#include <string>

namespace ot {
	
	class OT_SERVICEFOUNDATION_API_EXPORT AbstractModelNotifier {
	public:
		AbstractModelNotifier() {
		}
		virtual ~AbstractModelNotifier() {}

	private:
		AbstractModelNotifier(AbstractModelNotifier&) = delete;
		AbstractModelNotifier& operator = (AbstractModelNotifier&) = delete;
	};

}