//! @file AbstractUiNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date March 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <string>

namespace ot {
	
	class OT_SERVICEFOUNDATION_API_EXPORTONLY AbstractUiNotifier {
		OT_DECL_NOCOPY(AbstractUiNotifier)
		OT_DECL_NOMOVE(AbstractUiNotifier)
	public:
		AbstractUiNotifier() {};
		virtual ~AbstractUiNotifier() {};

		//! @brief Will be called when a key sequence was activated in the ui
		//! @param _keySequence The key sequence that was activated
		virtual void shortcutActivated(const std::string& _keySquence) {};
	};

}