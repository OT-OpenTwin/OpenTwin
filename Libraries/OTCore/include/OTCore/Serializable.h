//! @file Serializable.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/CoreAPIExport.h"
#include "OTCore/OTClassHelper.h"

namespace ot {

	//! @class Serializable
	//! @brief The Serializable class is the default interface of serializable objects.
	//! It provides the default virtual addToJsonObject and setFromJsonObject methods.
	class OT_CORE_API_EXPORTONLY Serializable {
		OT_DECL_DEFCOPY(Serializable)
		OT_DECL_DEFMOVE(Serializable)
	public:
		Serializable() = default;
		virtual ~Serializable() = default;

		//! @brief Serialize the object data into the provided JSON object by using the provided allocator.
		//! @param _jsonObject JSON object value reference to write the data to.
		//! @param _allocator Allocator to use when writing data.
		//! @ref Serializable::addToJsonObject
		virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const = 0;

		//! @brief Set the data by deserializing the object.
		//! Set the object contents from the provided JSON object.
		//! @param _jsonObject JSON object containing the information.
		//! @ref Serializable::setFromJsonObject
		virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) = 0;
	};
}