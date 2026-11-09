// @otlicense
// File: GlobalEventLogger.h
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
#include "OTCore/CoreTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtGui/qevent.h>

namespace ot
{
	
	class OT_WIDGETS_API_EXPORT GlobalEventLogger : public QObject
	{
		OT_DECL_NOCOPY(GlobalEventLogger)
		OT_DECL_NOMOVE(GlobalEventLogger)
		OT_DECL_NODEFAULT(GlobalEventLogger)
	public:
		static void install(const std::initializer_list<QEvent::Type>& _eventTypes);

		virtual bool eventFilter(QObject* _obj, QEvent* _event) override;
	private:
		explicit GlobalEventLogger(const std::initializer_list<QEvent::Type>& _eventTypes);

		bool m_enabledEventTypes[static_cast<int>(QEvent::User)];
	};

}
