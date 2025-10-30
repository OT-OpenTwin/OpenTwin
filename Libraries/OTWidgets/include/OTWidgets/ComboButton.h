// @otlicense
// File: ComboButton.h
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
#include "OTWidgets/PushButton.h"

class QMenu;
class QAction;

namespace ot {


	class OT_WIDGETS_API_EXPORT ComboButton : public PushButton {
		Q_OBJECT
		OT_DECL_NOCOPY(ComboButton)
	public:
		ComboButton(QWidget* _parent = (QWidget*)nullptr);
		ComboButton(const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		ComboButton(const QString& _text, const QStringList& _items, QWidget* _parent = (QWidget*)nullptr);
		virtual ~ComboButton();

		virtual void mousePressEvent(QMouseEvent* _event) override;

		void setItems(const QStringList& _items);

	Q_SIGNALS:
		void textChanged(void);

	private Q_SLOTS:
		void slotActionTriggered(QAction* _action);

	private:
		void ini(void);

		QMenu* m_menu;
	};

}
