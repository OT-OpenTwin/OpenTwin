#pragma once

#include "OTSystem/SystemAPIExport.h"

#include <string>

namespace ot {

	//! @namespace url
	namespace url {

		//! @brief Will encode the string given as argument such that it can be used in URLs.
		//! @param _str The string to be encoded
		//! @return The URL safe encoded string
		OT_SYS_API_EXPORT std::string urlEncode(std::string _str);
	}
}
