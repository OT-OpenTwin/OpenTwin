// @otlicense
// File: GradientPainterStop2D.h
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

namespace ot {

	class OT_GUI_API_EXPORT GradientPainterStop2D : public ot::Serializable {
	public:
		GradientPainterStop2D();
		GradientPainterStop2D(double _pos, const ot::Color& _color);
		GradientPainterStop2D(const GradientPainterStop2D& _other);
		virtual ~GradientPainterStop2D();

		GradientPainterStop2D& operator = (const GradientPainterStop2D& _other);
		bool operator == (const GradientPainterStop2D& _other) const;
		bool operator != (const GradientPainterStop2D& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Set postion of the gradient stop (0.0 - 1.0)
		void setPos(double _pos) { m_pos = _pos; };
		double getPos(void) const { return m_pos; };

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& getColor(void) const { return m_color; };

	private:
		double    m_pos;
		ot::Color m_color;
	};
}

