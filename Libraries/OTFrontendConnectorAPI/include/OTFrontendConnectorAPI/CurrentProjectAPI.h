//! @file CurrentProjectAPI.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTFrontendConnectorAPI/OTFrontendConnectorAPIExport.h"

// std header
#include <string>

namespace ot {
	class OT_FRONTEND_CONNECTOR_API_EXPORT CurrentProjectAPI {
	public:
		static void activateModelVersion(const std::string& _versionName);

	protected:
		virtual void activateModelVersionAPI(const std::string& _versionName) = 0;

		CurrentProjectAPI();
		virtual ~CurrentProjectAPI();
	private:
		static CurrentProjectAPI* instance();
		static CurrentProjectAPI*& getInstanceReference();
	};
}