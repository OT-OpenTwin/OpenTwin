// @otlicense
// File: Frontend.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

// std header
#include <map>
#include <list>
#include <string>

namespace ot {

	//! @class Frontend
	//! @brief Frontend request layer.
	//! The Frontend prvoides a set methods that may be used to send requests to the frontend.
	class OT_GUIAPI_API_EXPORT Frontend {
		OT_DECL_NOCOPY(Frontend)
		OT_DECL_NOMOVE(Frontend)
	public:

		// ###########################################################################################################################################################################################################################################################################################################################

		// File operations

		//! @brief Requests a file for reading from the frontend.
		//! @param _callbackAction The action to be called when the file has been selected.
		//! The will receive the following parameters:
		//! - OT_ACTION_PARAM_FILE_Mask: The file mask that was used in the request.
		//! - OT_ACTION_PARAM_Info: The additional info that was used in the request.
		//! - OT_ACTION_PARAM_FILE_Mode: Only if _loadContent was true.
		//! - OT_ACTION_PARAM_FILE_Content: The GridFSFileInfo config holding the file information if _loadContent was true (Array if _loadMultiple was true).
		//! - OT_ACTION_PARAM_FILE_OriginalName: The original file path as selected by the user (Array if _loadMultiple was true).
		//! @param _dialogTitle The title of the file dialog.
		//! @param _fileMask The file mask to be used in the dialog.
		//! @param _loadContent Whether the file content should be loaded and sent to the service.
		//! @param _loadMultiple Whether multiple files may be selected.
		//! @param _additionalInfo Additional info that will be sent back with the callback action.
		//! @return True if the request was sent successfully, false otherwise.
		static bool requestFileForReading(const std::string& _callbackAction, const std::string& _dialogTitle, const std::string& _fileMask, bool _loadContent, bool _loadMultiple, const std::string& _additionalInfo = std::string());

		static bool requestFileForWriting(const std::string& _callbackAction, const std::string& _dialogTitle, const std::string& _fileMask, const std::string& _additionalInfo = std::string());

		static bool writeDataToFile(const std::string& _dialogTitle, const std::string& _filePath, const std::string& _content, unsigned long long _uncompressedContentLength);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private

	private:
		
		Frontend() {};
		~Frontend() {};
	};

}
