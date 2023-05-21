//! @file AbstractInterpreterNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"

namespace aci {
	class OTSCI_API_EXPORT AbstractInterpreterNotifier {
	public:
		AbstractInterpreterNotifier() {}
		virtual ~AbstractInterpreterNotifier() {}

		virtual void disableInput(void) = 0;
		virtual void enableInput(void) = 0;
		virtual void shutdown(void) = 0;
	};
}