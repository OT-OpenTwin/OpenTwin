#pragma once

// Foundation header
#include "OTServiceFoundation/FoundationAPIExport.h"

// C++ header
#include <string>

namespace ot {
	
	class OT_SERVICEFOUNDATION_API_EXPORT AbstractUiNotifier {
	public:
		AbstractUiNotifier() {}
		virtual ~AbstractUiNotifier() {}

		//! @brief Will be called when a key sequence was activated in the ui
		//! @param _keySequence The key sequence that was activated
		virtual void shortcutActivated(const std::string& _keySquence) {};

	private:
		AbstractUiNotifier(AbstractUiNotifier&) = delete;
		AbstractUiNotifier& operator = (AbstractUiNotifier&) = delete;
	};

}