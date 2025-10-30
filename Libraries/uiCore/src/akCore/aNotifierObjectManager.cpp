// @otlicense
// File: aNotifierObjectManager.cpp
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

// AK header
#include <akCore/aException.h>
#include <akCore/aNotifierObjectManager.h>
#include <akGui/aObjectManager.h>

ak::aNotifierObjectManager::aNotifierObjectManager(
	aObjectManager *					_manager
) {
	try {
		if (_manager == nullptr) { throw aException("Is nullptr", "Check manager", aException::exceptionType::Nullptr); }
		m_manager = _manager;
	}
	catch (const aException & e) { throw aException(e, "aNotifierObjectManager::aNotifierObjectManager()"); }
	catch (const std::exception & e) { throw aException(e.what(), "aNotifierObjectManager::aNotifierObjectManager()"); }
	catch (...) { throw aException("Unknown error", "aNotifierObjectManager::aNotifierObjectManager()"); }
}

ak::aNotifierObjectManager::~aNotifierObjectManager(void) {}

// *****************************************************************************************

// Message IO

void ak::aNotifierObjectManager::notify(
	UID							_senderId,
	eventType					_event,
	int							_info1,
	int							_info2
) {
	try {
		if (_event == etDestroyed) {
			assert(isEnabled()); // Message received even if notifier was disabled
			m_manager->creatorDestroyed(_senderId);
		}
	}
	catch (const aException & e) { throw aException(e, "ak::notifierObjectManager::notify()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::notifierObjectManager::notify()"); }
	catch (...) { throw aException("Unknown error", "ak::notifierObjectManager::notify()"); }
}
