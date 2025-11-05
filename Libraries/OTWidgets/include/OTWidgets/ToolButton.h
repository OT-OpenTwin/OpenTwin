// @otlicense
// File: ToolButton.h
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

// Qt header
#include <QtWidgets/qtoolbutton.h>

namespace ot {

	class Action;

	class OT_WIDGETS_API_EXPORT ToolButton : public QToolButton, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(ToolButton)
		OT_DECL_NOMOVE(ToolButton)
		OT_DECL_NODEFAULT(ToolButton)
	public:
		explicit ToolButton(QWidget* _parent);
		explicit ToolButton(const QString& _text, QWidget* _parent);
		explicit ToolButton(const QIcon& _icon, const QString& _text, QWidget* _parent);

		virtual ~ToolButton();

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		// #######################################################################################################

	private:
		//! @brief Initializes the components of this toolButton
		void ini(void);
		
		Action* m_action;
	};
}
