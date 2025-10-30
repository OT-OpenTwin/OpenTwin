// @otlicense
// File: PropertyInputString.h
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
#include "OTWidgets/PropertyInput.h"

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	class LineEdit;

	class OT_WIDGETS_API_EXPORT PropertyInputString : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputString)
	public:
		PropertyInputString(const QString& _text = QString());
		virtual ~PropertyInputString();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue(void) const override;
		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;
		virtual Property* createPropertyConfiguration(void) const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput(void) override;

		void setText(const QString& _text);
		const QString& getCurrentText(void) const { return m_text; };

	private Q_SLOTS:
		void lclValueChanged(void);

	private:
		LineEdit* m_lineEdit;
		QString m_text;
	};

}