// @otlicense
// File: KeySequence.h
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
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"

#pragma warning(disable : 4251)

namespace ot {

	class OT_GUI_API_EXPORT KeySequence {
	public:
		KeySequence();
		KeySequence(BasicKey _key);
		KeySequence(BasicKey _key, BasicKey _key2);
		KeySequence(BasicKey _key, BasicKey _key2, BasicKey _key3);
		KeySequence(BasicKey _key, BasicKey _key2, BasicKey _key3, BasicKey _key4);
		KeySequence(const std::list<BasicKey>& _keys);
		~KeySequence();

		operator std::string(void) const;

	private:
		std::list<BasicKey> m_keys;

	};

}