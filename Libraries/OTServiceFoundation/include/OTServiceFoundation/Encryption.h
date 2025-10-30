// @otlicense

#pragma once

// OpenTwin header
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <string>

namespace ot {

	class Encryption {
	public:
		//! @brief Encrypt the provided string.
		//! @callergraph
		//! @callgraph
		OT_SERVICEFOUNDATION_API_EXPORT static std::string encryptString(const std::string& _str);

		//! @brief Decrypt the provided string.
		//! @callergraph
		//! @callgraph
		OT_SERVICEFOUNDATION_API_EXPORT static std::string decryptString(const std::string& _str);

	private:
		Encryption() = delete;
	};
}