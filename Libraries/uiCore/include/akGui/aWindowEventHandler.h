// @otlicense

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>

namespace ak {
	class UICORE_API_EXPORT aWindowEventHandler {
	public:
		aWindowEventHandler() {}

		virtual ~aWindowEventHandler() {}

		// ###################################################################################

		// Event handling routines

		//! @brief This function will be called by the window when the closeEvent occurs
		//! If the return value is true the closeEvent will be acceptet otherwise ignored 
		virtual bool closeEvent(void) { return true; }
	};
}