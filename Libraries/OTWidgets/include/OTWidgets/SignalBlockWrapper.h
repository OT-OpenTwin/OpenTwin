// @otlicense
// File: SignalBlockWrapper.h
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
#include <QtCore/qobject.h>

namespace ot {
	
	//! @brief The SignalBlockWrapper is a RAII wrapper for blocking signals of a given QObject.
	class OT_WIDGETS_API_EXPORT SignalBlockWrapper {
		OT_DECL_NOCOPY(SignalBlockWrapper)
		OT_DECL_NODEFAULT(SignalBlockWrapper)
	public:
		//! @brief Constructor.
		//! The currently set "block signals" state of the given object will be stored and the signals will be blocked.
		SignalBlockWrapper(QObject* _object) : m_object(_object), m_initialState(false) {
			if (m_object) {
				m_initialState = m_object->signalsBlocked();
				m_object->blockSignals(true);
			}
		}

		//! @brief Destructor.
		//! The "blocked signals" state of the provided object will be set back to the initial value, any previous changes will be ignored.
		~SignalBlockWrapper() {
			if (m_object) {
				m_object->blockSignals(m_initialState);
			}
		}
	private:
		QObject* m_object;
		bool m_initialState;
	};

}