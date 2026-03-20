// @otlicense
// File: MenuSeparatorCfg.cpp
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
#include "OTGui/Menu/MenuSeparatorCfg.h"
#include "OTGui/Menu/MenuEntryCfgFactory.h"

static ot::MenuEntryCfgFactoryRegistrar<ot::MenuSeparatorCfg> separatorRegistarar(ot::MenuSeparatorCfg::className());

ot::MenuEntryCfg* ot::MenuSeparatorCfg::createCopy() const {
	return new MenuSeparatorCfg(*this);
}

bool ot::MenuSeparatorCfg::isEqual(const MenuEntryCfg* _other) const
{
	const MenuSeparatorCfg* other = dynamic_cast<const MenuSeparatorCfg*>(_other);
	return other != nullptr;
}
