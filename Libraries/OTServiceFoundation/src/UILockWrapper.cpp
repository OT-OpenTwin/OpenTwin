// @otlicense
// File: UILockWrapper.cpp
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

#include "OTServiceFoundation/UILockWrapper.h"

ot::UILockWrapper::UILockWrapper(ot::components::UiComponent* _uiComponent, const ot::LockTypes& _flags)
	: m_lockFlags(_flags), m_uiComponent(_uiComponent) 
{
	OTAssertNullptr(m_uiComponent);
	if (m_uiComponent) {
		OT_LOG_D("Applying UI lock.");
		m_uiComponent->lockUI(m_lockFlags);
	}
}

ot::UILockWrapper::~UILockWrapper() {
	if (m_uiComponent) {
		OT_LOG_D("Release UI lock.");
		m_uiComponent->unlockUI(m_lockFlags);
	}
}