// @otlicense
// File: StatusBarManager.h
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
#include "OTWidgets/AbstractStatusBar.h"

class QWidget;

namespace ot {

	class OT_WIDGETS_API_EXPORT StatusBarManager : public AbstractStatusBar {
		OT_DECL_NOCOPY(StatusBarManager)
		OT_DECL_NOMOVE(StatusBarManager)
		OT_DECL_NODEFAULT(StatusBarManager)
	public:
		explicit StatusBarManager(QWidget* _parent);
		virtual ~StatusBarManager();

		virtual QStatusBar* getStatusBar(void) override;
		virtual const QStatusBar* getStatusBar(void) const override;

	private:
		QStatusBar* m_statusBar;
	};

}