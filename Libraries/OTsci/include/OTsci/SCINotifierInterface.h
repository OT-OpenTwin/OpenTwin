//! @file SCINotifierInterface.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"

namespace ot {

	class OTSCI_API_EXPORT SCINotifierInterface {
	public:
		SCINotifierInterface() {};
		virtual ~SCINotifierInterface() {};

		//! @brief Disables the user controls
		//! The interface implementation should disable all inputs (no more messages should be dispatched)
		virtual void disableInput(void) = 0;

		//! @brief Enable the user controls
		//! The interace implementation should enable all inputs (messages may be dispatched (again))
		virtual void enableInput(void) = 0;

		//! @brief Shutdown the SCI wrapper (sci internally still ready to work)
		virtual void shutdown(void) = 0;
	};
}