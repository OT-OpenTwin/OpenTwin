//! @file JSONDocument.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <string>

namespace ot {

	//! @class JsonDocument
	//! @brief JSON document.
	class OT_CORE_API_EXPORT JsonDocument : public rapidjson::Document {
		OT_DECL_NOCOPY(JsonDocument)
		OT_DECL_DEFMOVE(JsonDocument)
	public:
		//! @brief Constructor
		//! Set the document as object
		JsonDocument();

		//! @brief Constructor
		//! @param _type The type for the json document
		JsonDocument(rapidjson::Type _type);

		virtual ~JsonDocument() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		const JsonDocument& constRef(void) const { return *this; };

		ConstJsonObject GetConstObject() const { return this->GetObject(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		bool fromJson(const std::string& _json) { return this->fromJson(_json.c_str()); };
		bool fromJson(const char* _fromJson);

		std::string toJson(void) const;
	};

}