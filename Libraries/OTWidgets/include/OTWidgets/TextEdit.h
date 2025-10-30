// @otlicense
// File: TextEdit.h
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
#include <QtWidgets/qtextedit.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT TextEdit : public QTextEdit, public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(TextEdit)
		OT_DECL_NOMOVE(TextEdit)
	public:
		TextEdit(QWidget* _parent = (QWidget*)nullptr);
		virtual ~TextEdit();

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

	};

}
