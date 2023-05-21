//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"
#include <aci/AbstractOSHandler.h>

namespace aci {
	class OTSCI_API_EXPORT OS {
	public:
		static OS * instance(void);
		static void clearInstance(void);

		void attachOSHandler(AbstractOSHandler * _handler);
		AbstractOSHandler * handler(void) { return m_handler; }

	private:

		AbstractOSHandler *		m_handler;

	};
}