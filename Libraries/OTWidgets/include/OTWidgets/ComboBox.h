// @otlicense
// File: ComboBox.h
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
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qcombobox.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ComboBox : public QComboBox, public WidgetBase {
		Q_OBJECT
	public:
		ComboBox(QWidget* _parent = (QWidget*)nullptr);
		virtual ~ComboBox() {};

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		bool isPopupVisible() const { return m_popupVisible; };

		virtual void showPopup() override;
		virtual void hidePopup() override;

	Q_SIGNALS:
		void returnPressed();

	public Q_SLOTS:
		void togglePopup();

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;

	private:
		bool m_popupVisible;
	};

}
