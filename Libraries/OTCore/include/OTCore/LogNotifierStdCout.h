// @otlicense
// File: LogNotifierStdCout.h
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
#include "OTCore/AbstractLogNotifier.h"

namespace ot {

	//! @brief Used to write created log messages to std cout in a way a human could read it.
	class OT_CORE_API_EXPORT LogNotifierStdCout : public AbstractLogNotifier {
	public:
		virtual ~LogNotifierStdCout() {};

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) override;
	};

}