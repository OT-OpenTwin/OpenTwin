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

		//! @brief Unzip and decrypt the provided string.
		//! @callergraph
		//! @callgraph
		OT_SERVICEFOUNDATION_API_EXPORT static std::string decryptAndUnzipString(const std::string& _str, uint64_t uncompressedLength);

		//! @brief Encrypt the provided string and zip it.
		//! @callergraph
		//! @callgraph
		OT_SERVICEFOUNDATION_API_EXPORT static std::string encryptAndZipString(const std::string& _str);

	private:
		Encryption() = delete;
	};
}