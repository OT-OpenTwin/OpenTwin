// @otlicense
// File: aTtbContainer.cpp
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
#include <akCore/aMessenger.h>
#include <akGui/aTtbContainer.h>

ak::aTtbContainer::aTtbContainer(
	aMessenger *			_messenger,
	objectType				_type,
	UID						_uid
) : aWidget(_type, _uid),
	m_messenger(nullptr),
	m_isEnabled(true)
{
	assert(_messenger != nullptr); // Is nullptr
	m_messenger = _messenger;
}

ak::aTtbContainer::~aTtbContainer() {}

ak::aTtbContainer * ak::aTtbContainer::getSubContainer(
	const QString &				_text
) {
	for (auto itm : m_subContainer) { if (itm->text() == _text) { return itm; } }
	return nullptr;
}

int ak::aTtbContainer::subContainerCount(void) const { return m_subContainer.size(); }
