// @otlicense
// File: AbstractLogNotifier.h
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
#include "OTCore/LogMessage.h"

namespace ot {

	//! @brief Used to receive every log message that is generated.
	class OT_CORE_API_EXPORTONLY AbstractLogNotifier {
	public:
		AbstractLogNotifier() : m_customDelete(false) {};
		virtual ~AbstractLogNotifier() {};

		//! @brief Will set the delete later flag.
		//! If delete later is set, the creator keeps ownership of this object even after it is added to the LogDispatcher.
		void setCustomDeleteLogNotifier(bool _customDelete = true) { m_customDelete = _customDelete; };

		//! @brief Returns true if the delete later mode is set.
		bool getCustomDeleteLogNotifier() const { return m_customDelete; };

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) = 0;

	private:
		bool m_customDelete; //! @brief If delete later is set, the creator keeps ownership of this object even after it is added to the LogDispatcher.
	};

}