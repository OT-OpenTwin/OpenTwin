//! @file Painter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OTCore/Serializable.h"
#include "OTCore/SimpleFactory.h"

namespace ot {

	class OT_GUI_API_EXPORT Painter2D : public ot::Serializable, public ot::SimpleFactoryObject {
	public:
		Painter2D() {};
		virtual ~Painter2D() {};

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		
		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		virtual std::string generateQss(void) const = 0;

		//! @brief Creates a copy of this painter
		Painter2D* createCopy(void) const;

	private:
		Painter2D(const Painter2D&) = delete;
		Painter2D& operator = (const Painter2D&) = delete;
	};

}