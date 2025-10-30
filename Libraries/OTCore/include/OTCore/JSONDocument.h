// @otlicense
// File: JSONDocument.h
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
#include "OTCore/CoreTypes.h"
#include "OTCore/JSONTypes.h"

// std header
#include <string>

namespace ot {

	//! @class JsonDocument
	//! @brief JSON document.
	class JsonDocument : public rapidjson::Document {
		OT_DECL_NOCOPY(JsonDocument)
		OT_DECL_DEFMOVE(JsonDocument)
	public:
		//! @brief Constructor
		//! Set the document as object
		JsonDocument();

		//! @brief Constructor
		//! @param _type The type for the json document
		explicit JsonDocument(rapidjson::Type _type);

		~JsonDocument() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		const JsonDocument& constRef() const { return *this; };

		ConstJsonObject getConstObject() const { return rapidjson::Document::GetObject(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		bool fromJson(const std::string& _json) { return this->fromJson(_json.c_str()); };
		bool fromJson(const char* _fromJson);

		std::string toJson() const;
	};

}

#include "OTCore/JSONDocument.hpp"