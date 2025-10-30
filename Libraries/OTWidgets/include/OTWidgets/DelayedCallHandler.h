// @otlicense
// File: DelayedCallHandler.h
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
#include <QtCore/qtimer.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT DelayedCallHandler : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(DelayedCallHandler)
	public:
		DelayedCallHandler(int _delayInMs = 0);
		
		void setDelay(int _delayInMs);
		int getDelay(void) const;

		void callDelayed(bool _waitUntilCalled = false);
		void stop(void);

	Q_SIGNALS:
		void timeout(void);

	private Q_SLOTS:
		void slotTimeout(void);

	private:
		QTimer m_timer;
		bool m_waitForCall;
	};

}