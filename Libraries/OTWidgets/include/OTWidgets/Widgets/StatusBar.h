// @otlicense
// File: StatusBar.h
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
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qstatusbar.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT StatusBar : public QStatusBar, public WidgetBase
	{
		Q_OBJECT
		OT_DECL_NOCOPY(StatusBar)
		OT_DECL_NOMOVE(StatusBar)
		OT_DECL_NODEFAULT(StatusBar)
	public:
		StatusBar(QWidget* _parent);
		virtual ~StatusBar();
		
		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

	};
}