// @otlicense
// File: Painter2D.h
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
#include "OTCore/Color.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

#define OT_JSON_MEMBER_Painter2DType "P2Type"

namespace ot {

	class OT_GUI_API_EXPORT Painter2D : public ot::Serializable {
	public:
		Painter2D() {};
		virtual ~Painter2D() {};

		//! @brief Returns the unique Painter2D type name that is used in the Painter2DFactory.
		virtual std::string getFactoryKey() const = 0;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		
		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		virtual std::string generateQss() const = 0;

		//! @brief Generates an SVG paint server with the specified ID.
		//! The paint server ID is used to reference the paint server in SVG elements.
		//! The string returned contains the SVG definitions needed for the paint server.
		//! @param _id The ID to use for the paint server.
		//! @return A string containing the SVG definitions for the paint server.
		virtual std::string generateSvgColorString(const std::string& _id) const = 0;

		virtual ot::Color getDefaultColor() const = 0;

		//! @brief Returns true if this painter is equal to the other painter.
		virtual bool isEqualTo(const Painter2D* _other) const = 0;

		//! @brief Creates a copy of this painter
		Painter2D* createCopy() const;

	private:
		Painter2D(const Painter2D&) = delete;
		Painter2D& operator = (const Painter2D&) = delete;
	};

}