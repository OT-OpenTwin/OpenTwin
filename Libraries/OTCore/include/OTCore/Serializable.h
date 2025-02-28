//! @file Serializable.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OT header
#include "OTCore/JSON.h"
#include "OTCore/CoreAPIExport.h"

namespace ot {

	//! \class Serializable.
	//! \brief The Serializable class is the default interface of serializable objects.
	class OT_CORE_API_EXPORTONLY Serializable {
	public:
		Serializable() {};
		Serializable(const Serializable&) = default;
		Serializable(Serializable&&) noexcept = default;
		virtual ~Serializable() {};

		Serializable& operator = (const Serializable&) = default;
		Serializable& operator = (Serializable&&) noexcept = default;

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const = 0;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) = 0;
	};
}