// @otlicense
// File: RadialGradientPainter2D.h
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
#include "OTCore/Point2D.h"
#include "OTGui/GradientPainter2D.h"
#include "OTGui/GradientPainterStop2D.h"

#define OT_FactoryKey_RadialGradientPainter2D "OT_P2DRGrad"

namespace ot {

	class OT_GUI_API_EXPORT RadialGradientPainter2D : public ot::GradientPainter2D {
		OT_DECL_NOCOPY(RadialGradientPainter2D)
	public:
		RadialGradientPainter2D();
		virtual ~RadialGradientPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_RadialGradientPainter2D); };

		virtual std::string generateQss(void) const override;

		virtual bool isEqualTo(const Painter2D* _other) const override;

		//! @brief Set the center point for the gradient
		void setCenterPoint(const ot::Point2DD& _center) { m_center = _center; };
		const ot::Point2DD& getCenterPoint(void) const { return m_center; }

		void setCenterRadius(double _r) { m_centerRadius = _r; };
		double getCenterRadius(void) const { return m_centerRadius; };

		//! @brief Set the focal point for the gradient
		void setFocalPoint(const ot::Point2DD& _focal);
		const ot::Point2DD& getFocalPoint(void) const { return m_focal; };

		//! @brief Set the focal radius
		//! Note that the focal point needs to be set
		void setFocalRadius(double _r) { m_focalRadius = _r; };
		double getFocalRadius(void) const { return m_focalRadius; };

		bool isFocalPointSet(void) const { return m_focalSet; };

	private:
		ot::Point2DD m_center;
		double m_centerRadius;
		ot::Point2DD m_focal;
		double m_focalRadius;
		bool m_focalSet;
	};

}