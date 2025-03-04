//! @file Serializable.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/CoreAPIExport.h"

namespace ot {

	//! @class Serializable
	//! @brief The Serializable class is the default interface of serializable objects.
	//! It provides the default virtual addToJsonObject and setFromJsonObject methods.
	class OT_CORE_API_EXPORTONLY Serializable {
	public:
		Serializable() {};
		Serializable(const Serializable&) {};
		Serializable(Serializable&&) noexcept {};
		virtual ~Serializable() {};

		Serializable& operator = (const Serializable&) { return *this; };
		Serializable& operator = (Serializable&&) noexcept { return *this; };

		//! @brief Serialize the object data into the provided object by using the provided allocator.
		//! @param _object Json object value reference to write the data to.
		//! @param _allocator Allocator to use when writing data.
		//! @ref Serializable::addToJsonObject
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const = 0;

		//! @brief Set the data by deserializing the object.
		//! Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @ref Serializable::setFromJsonObject
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) = 0;
	};
}