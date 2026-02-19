// @otlicense
// File: PropertyInputStringList.h
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
#include <QtCore/qtimer.h>

namespace ot {

	class ComboBox;
	class ComboButton;

	class OT_WIDGETS_API_EXPORT PropertyInputStringList : public PropertyInput {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInputStringList)
		OT_DECL_NOMOVE(PropertyInputStringList)
		OT_DECL_NODEFAULT(PropertyInputStringList)
	public:
		explicit PropertyInputStringList(QWidget* _parent);
		virtual ~PropertyInputStringList();

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) override;
		virtual QVariant getCurrentValue() const override;
		virtual QWidget* getQWidget() override;
		virtual const QWidget* getQWidget() const override;
		virtual Property* createPropertyConfiguration() const override;
		virtual bool setupFromConfiguration(const Property* _configuration) override;
		virtual void focusPropertyInput() override;

		void setCurrentText(const QString& _text);
		QString getCurrentText() const;
		QStringList getPossibleSelection() const;

	private Q_SLOTS:
		void slotTextInputChanged();

	private:
		QTimer m_delayTimer;
		QString m_currentText;
		QWidget* m_parentWidget;
		ComboBox* m_comboBox;
		ComboButton* m_comboButton;
	};

}