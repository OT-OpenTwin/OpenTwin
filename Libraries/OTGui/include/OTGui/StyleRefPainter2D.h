// @otlicense
// File: StyleRefPainter2D.h
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
#include "OTGui/Painter2D.h"
#include "OTGui/ColorStyleTypes.h"

// std header
#include <string>

#define OT_FactoryKey_StyleRefPainter2D "OT_P2DStyR"

#pragma warning(disable:4251)

namespace ot {

	//! @class StyleRefPainter2D
	//! @brief The StyleRefPainter2D can be used to reference a ColorStyleValue in the active ColorStyle.
	//! @note Note that the reference is only evaluated in the frontend by the QtFactory.
	//! *     When the active ColorStyle changes every GUI element working with Painter2D should reevaluate the Painter2D.
	class OT_GUI_API_EXPORT StyleRefPainter2D : public ot::Painter2D {
		OT_DECL_NOCOPY(StyleRefPainter2D)
	public:
		StyleRefPainter2D(ColorStyleValueEntry _referenceKey = ColorStyleValueEntry::WidgetBackground);
		virtual ~StyleRefPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_StyleRefPainter2D); };

		virtual std::string generateQss(void) const override;

		virtual ot::Color getDefaultColor(void) const override;

		virtual bool isEqualTo(const Painter2D* _other) const override;

		//! @brief Sets the key of the ColorStyleValue that should be referenced.
		//! @param _referenceKey Key to set.
		void setReferenceKey(ColorStyleValueEntry _referenceKey) { m_reference = _referenceKey; };

		//! @brief Key of the ColorStyleValue that is referenced.
		ColorStyleValueEntry getReferenceKey(void) const { return m_reference; };

	private:
		ColorStyleValueEntry m_reference; //! @brief ColorStyleValue key.
	};

}