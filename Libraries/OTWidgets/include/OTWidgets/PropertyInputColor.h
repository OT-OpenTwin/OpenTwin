// @otlicense
// File: PropertyInputColor.h
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
#include "OTWidgets/PropertyInput.h"

// Qt header
#include <QtGui/qcolor.h>

namespace ot {

	class ColorPickButton;

	class OT_WIDGETS_API_EXPORT PropertyInputColor : public PropertyInput {
		OT_DECL_NOCOPY(PropertyInputColor)
		OT_DECL_NOMOVE(PropertyInputColor)
		OT_DECL_NODEFAULT(PropertyInputColor)
	public:
		explicit PropertyInputColor(QWidget* _parent);
		virtual ~PropertyInputColor();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput(void) override;

		void setColor(const Color& _color);
		void setColor(const QColor& _color);
		Color getOTColor(void) const;
		QColor getColor(void) const;

		ColorPickButton* getColorPickButton(void) const { return m_colorBtn; };

	private:
		ColorPickButton* m_colorBtn;
	};

}