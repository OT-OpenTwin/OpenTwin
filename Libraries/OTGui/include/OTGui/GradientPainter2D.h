// @otlicense
// File: GradientPainter2D.h
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

#include "OTCore/Point2D.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Painter2D.h"
#include "OTGui/GradientPainterStop2D.h"

namespace ot {

	class OT_GUI_API_EXPORT GradientPainter2D : public ot::Painter2D {
		OT_DECL_NOCOPY(GradientPainter2D)
	public:
		GradientPainter2D();
		GradientPainter2D(const std::vector<GradientPainterStop2D>& _stops);
		virtual ~GradientPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		virtual ot::Color getDefaultColor(void) const override;

		virtual bool isEqualTo(const Painter2D* _other) const override;

		inline void addStop(double _pos, const ot::Color& _color) { this->addStop(GradientPainterStop2D(_pos, _color)); };
		void addStop(const GradientPainterStop2D& _stop);
		void addStops(const std::vector<GradientPainterStop2D>& _stops);
		void setStops(const std::vector<GradientPainterStop2D>& _stops) { m_stops = _stops; };
		const std::vector<GradientPainterStop2D>& getStops(void) const { return m_stops; };

		void setSpread(GradientSpread _spread) { m_spread = _spread; };
		GradientSpread getSpread(void) const { return m_spread; };

	protected:
		void addStopsAndSpreadToQss(std::string& _targetString) const;

	private:
		std::vector<GradientPainterStop2D> m_stops;
		GradientSpread m_spread;
	};

}