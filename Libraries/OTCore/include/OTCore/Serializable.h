// @otlicense

//! @file Serializable.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

namespace ot {

	//! @class Serializable
	//! @brief The Serializable class is the default interface of serializable objects.
	//! It provides the default virtual addToJsonObject and setFromJsonObject methods.
	class OT_CORE_API_EXPORT Serializable {
		OT_DECL_DEFCOPY(Serializable)
		OT_DECL_DEFMOVE(Serializable)
	public:
		Serializable() = default;
		virtual ~Serializable() = default;

		//! @brief Serialize the object data into the provided JSON object by using the provided allocator.
		//! @param _jsonObject JSON object value reference to write the data to.
		//! @param _allocator Allocator to use when writing data.
		//! @ref Serializable::addToJsonObject
		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const = 0;

		//! @brief Set the data by deserializing the object.
		//! Set the object contents from the provided JSON object.
		//! @param _jsonObject JSON object containing the information.
		//! @ref Serializable::setFromJsonObject
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) = 0;

		//! @brief Create a JSON string from the object data.
		virtual std::string toJson() const;
	};
}