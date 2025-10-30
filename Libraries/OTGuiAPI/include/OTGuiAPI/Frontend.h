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
