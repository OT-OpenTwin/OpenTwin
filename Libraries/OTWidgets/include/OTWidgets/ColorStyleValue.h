// @otlicense
// File: ColorStyleValue.h
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
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qcolor.h>
#include <QtGui/qbrush.h>

// std header
#include <string>

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT ColorStyleValue : public Serializable {
	public:
		ColorStyleValue();
		ColorStyleValue(const ColorStyleValue& _other);
		~ColorStyleValue();

		ColorStyleValue& operator = (const ColorStyleValue& _other);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		void setEntryKey(ColorStyleValueEntry _key) { m_entryKey = _key; };
		ColorStyleValueEntry getEntryKey() const { return m_entryKey; };

		QString toQss() const;

		//! @brief Generates an SVG paint server with the specified ID.
		//! The paint server ID is used to reference the paint server in SVG elements.
		//! The string returned contains the SVG definitions needed for the paint server.
		//! @param _id The ID to use for the paint server.
		//! @return A byteArray string containing the SVG definitions for the paint server.
		QByteArray toSvgColorString(const std::string& _id) const;

		QColor toColor() const;
		QBrush toBrush() const;

		//! @brief Set the painter which will also set the brush
		//! The object takes ownership
		void setPainter(Painter2D* _painter);
		void setPainter(const Painter2D* _painter);
		Painter2D* painter() const { return m_painter; };

		void setColor(DefaultColor _color);
		void setColor(Color _color);

	private:
		ColorStyleValueEntry m_entryKey;
		Painter2D* m_painter;
	};

}
