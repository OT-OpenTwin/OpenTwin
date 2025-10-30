// @otlicense
// File: aUidManager.cpp
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
#include <akCore/aUidMangager.h>
#include <akCore/aException.h>

ak::aUidManager::aUidManager() : m_currentId(1) {}

ak::aUidManager::~aUidManager() {}

// ###########################################################

ak::UID ak::aUidManager::getId(void) {
	m_mutex.lock();
	ak::UID id = m_currentId++;
	if (id == 0xffffffffffffffff) {
		assert(0);
		m_mutex.unlock();
		throw aException("Max UID reached!", "aUidManager::getId()", aException::ArrayOutOfBounds);
	}
	m_mutex.unlock();
	return id;
}

void ak::aUidManager::reset(void) {
	m_mutex.lock();
	m_currentId = 1;
	m_mutex.unlock();
}

void ak::aUidManager::setLatestUid(ak::UID _uid) {
	m_mutex.lock();
	m_currentId = _uid;
	m_mutex.unlock();
}