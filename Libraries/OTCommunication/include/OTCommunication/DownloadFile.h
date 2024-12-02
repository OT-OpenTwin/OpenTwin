/*
 *	DownloadFile.h
 *
 *  Copyright (c) 2024 openTwin
 */

#pragma once

#include "OTCommunication/CommunicationAPIExport.h"
#include "OTCommunication/CommunicationTypes.h"

 // C++ header
#include <string>

namespace ot {
	namespace DownloadFile {

		//! @brief Download a file via HTTP from a server
		//! @param _fileUrl The URL of the file to be downloaded
		//! @param _fileName Name of the file where the data will be stored
		//! @param _tempFolder Return parameter containing the name of the temporary directory where the file is stored
		//! @param _error Return parameter containing an error description in case that an error occurred (return value = false)
		//! @param _timeout The timeout for contacting the server
		OT_COMMUNICATION_API_EXPORT bool download(
			const std::string &				_fileUrl,
			const std::string &				_fileName,
			std::string&					_tempFolder,
			std::string&					_error,
			int								_timeout = 3000
		);
	}
}