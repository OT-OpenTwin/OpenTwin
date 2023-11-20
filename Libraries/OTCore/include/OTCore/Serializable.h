/*
 *  Serializable.h
 *
 *  Created on: 24/02/2023
 *	Author: Alexander Kuester
 *  Copyright (c) 2023, OpenTwin
 */

#pragma once

// OT header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/rJSON.h"

namespace ot {

	class OT_CORE_API_EXPORT Serializable {
	public:
		Serializable() {};
		virtual ~Serializable() {};

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const = 0;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) = 0;
	};
}