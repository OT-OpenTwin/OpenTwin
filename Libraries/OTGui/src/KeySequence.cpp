// @otlicense
// File: KeySequence.cpp
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
#include "OTGui/KeySequence.h"

ot::KeySequence::KeySequence() {}

ot::KeySequence::KeySequence(BasicKey _key) {
	m_keys.push_back(_key);
}

ot::KeySequence::KeySequence(BasicKey _key, BasicKey _key2) {
	m_keys.push_back(_key);
	m_keys.push_back(_key2);
}

ot::KeySequence::KeySequence(BasicKey _key, BasicKey _key2, BasicKey _key3) {
	m_keys.push_back(_key);
	m_keys.push_back(_key2);
	m_keys.push_back(_key3);
}

ot::KeySequence::KeySequence(BasicKey _key, BasicKey _key2, BasicKey _key3, BasicKey _key4) {
	m_keys.push_back(_key);
	m_keys.push_back(_key2);
	m_keys.push_back(_key3);
	m_keys.push_back(_key4);
}

ot::KeySequence::KeySequence(const std::list<BasicKey>& _keys) : m_keys(_keys) {}

ot::KeySequence::~KeySequence() {}

ot::KeySequence::operator std::string(void) const {
	std::string str;
	for (BasicKey k : m_keys) {
		if (!str.empty()) str.append("+");
		str.append(toString(k));
	}
	return str;
}