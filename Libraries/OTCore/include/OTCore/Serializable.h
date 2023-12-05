/*
 *  Serializable.h
 *
 *  Created on: 24/02/2023
 *	Author: Alexander Kuester
 *  Copyright (c) 2023, OpenTwin
 */

#pragma once

// OT header
#include "OTCore/JSON.h"
#include "OTCore/CoreAPIExport.h"

namespace ot {

	class OT_CORE_API_EXPORTONLY Serializable {
	public:
		Serializable() {};
		virtual ~Serializable() {};

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const = 0;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) = 0;
	};
}