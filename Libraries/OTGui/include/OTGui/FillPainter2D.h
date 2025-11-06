// @otlicense
// File: FillPainter2D.h
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
#include "OTGui/Painter2D.h"

#define OT_FactoryKey_FillPainter2D "OT_P2DFill"

namespace ot {
	
	class OT_GUI_API_EXPORT FillPainter2D : public ot::Painter2D {
	public:
		FillPainter2D();
		FillPainter2D(ot::DefaultColor _color);
		FillPainter2D(int _r, int _g, int _b, int _a = 255);
		FillPainter2D(const ot::Color& _color);
		virtual ~FillPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey() const override { return std::string(OT_FactoryKey_FillPainter2D); };

		virtual std::string generateQss() const override;

		virtual std::string generateSvgColorString(const std::string& _id) const override;

		virtual ot::Color getDefaultColor() const override;

		virtual bool isEqualTo(const Painter2D* _other) const override;

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& getColor() const { return m_color; };

	private:
		ot::Color m_color;

		FillPainter2D(const FillPainter2D&) = delete;
		FillPainter2D& operator = (const FillPainter2D&) = delete;
	};

}