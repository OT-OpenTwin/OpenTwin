// @otlicense
// File: DelayedShowHideHandler.h
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
#include "OTWidgets/DelayedCallHandler.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT DelayedShowHideHandler : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(DelayedShowHideHandler)
	public:
		DelayedShowHideHandler();
		~DelayedShowHideHandler();

		void show(void);
		void setShowDelay(int _delayInMs) { m_showHandler.setDelay(_delayInMs); };
		int getShowDelay(void) const { return m_showHandler.getDelay(); };

		void hide(void);
		void setHideDelay(int _delayInMs) { m_hideHandler.setDelay(_delayInMs); };
		int getHideDelay(void) const { return m_hideHandler.getDelay(); };

		void stop(void);

	Q_SIGNALS:
		void showRequest(void);
		void hideRequest(void);

	private Q_SLOTS:
		void slotShow(void);
		void slotHide(void);

	private:
		DelayedCallHandler m_showHandler;
		DelayedCallHandler m_hideHandler;
	};

}