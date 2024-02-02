//! @file Encryption.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <string>

namespace ot {

	OT_SERVICEFOUNDATION_API_EXPORT std::string encryptString(const std::string& _str);
	OT_SERVICEFOUNDATION_API_EXPORT std::string decryptString(const std::string& _str);

}