// @otlicense
// File: WidgetBase.h
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
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qwidget.h>

namespace ot {

	class WidgetView;

	class OT_WIDGETS_API_EXPORTONLY WidgetBase {
	public:
		WidgetBase() : m_parentView(nullptr) {};
		virtual ~WidgetBase() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget() = 0;
		virtual const QWidget* getQWidget() const = 0;

		void setOTWidgetFlags(const WidgetFlags& _flags);
		const WidgetFlags& otWidgetFlags() const { return m_widgetFlags; };

		void setParentWidgetView(WidgetView* _view) { m_parentView = _view; };
		WidgetView* getParentWidgetView() const { return m_parentView; };

		void setSuccessForeground(bool _enabled = true);
		void setWarningForeground(bool _enabled = true);
		void setErrorForeground(bool _enabled = true);

	protected:
		virtual void otWidgetFlagsChanged(const WidgetFlags& _flags) {};

	private:
		WidgetFlags m_widgetFlags;
		WidgetView* m_parentView;
	};
}