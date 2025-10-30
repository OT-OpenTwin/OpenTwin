// @otlicense
// File: FrontendLogNotifier.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/FrontendLogNotifier.h"

ot::FrontendLogNotifier::FrontendLogNotifier(ApplicationBase* _app)
	: m_app(_app), m_flags(ERROR_LOG | WARNING_LOG | TEST_LOG)
{
	OTAssertNullptr(m_app);
	ot::LogDispatcher::instance().addReceiver(this);
}

ot::FrontendLogNotifier::~FrontendLogNotifier() {

}

void ot::FrontendLogNotifier::log(const LogMessage& _message) {
	if (_message.getFlags() & m_flags) {
		if (m_app->isUiConnected()) {
			OTAssertNullptr(m_app->getUiComponent());
			m_app->getUiComponent()->displayLogMessage(_message);
		}
	}
}
