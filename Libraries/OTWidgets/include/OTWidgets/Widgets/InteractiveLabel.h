// @otlicense
// File: InteractiveLabel.h
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
#include "OTWidgets/Widgets/Label.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT InteractiveLabel : public Label
	{
		Q_OBJECT
		OT_DECL_NOCOPY(InteractiveLabel)
		OT_DECL_NOMOVE(InteractiveLabel)
		OT_DECL_NODEFAULT(InteractiveLabel)
	public:
		explicit InteractiveLabel(QWidget* _parentWidget);
		explicit InteractiveLabel(const QString& _text, QWidget* _parentWidget);
		virtual ~InteractiveLabel() {};

	Q_SIGNALS:
		void mouseClicked(Qt::KeyboardModifiers _keyboardModifiers);
		void mouseDoubleClicked(Qt::KeyboardModifiers _keyboardModifiers);
		void mouseEntered();
		void mouseLeft();

	protected:
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void mouseDoubleClickEvent(QMouseEvent* _event) override;
		virtual void enterEvent(QEnterEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

	private:
		bool m_mouseIsPressed;
	};
}