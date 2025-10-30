// @otlicense
// File: MenuEntryCfgFactory.h
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
#include "OTCore/FactoryTemplate.h"
#include "OTGui/MenuEntryCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT MenuEntryCfgFactory : public FactoryTemplate<MenuEntryCfg> {
		OT_DECL_NOCOPY(MenuEntryCfgFactory)
	public:
		static MenuEntryCfgFactory& instance(void);

		static MenuEntryCfg* create(const ConstJsonObject& _jsonObject);

	private:
		MenuEntryCfgFactory();
		~MenuEntryCfgFactory();
	};

	template <class T>
	class OT_GUI_API_EXPORT MenuEntryCfgFactoryRegistrar : public FactoryRegistrarTemplate<MenuEntryCfgFactory, T> {
	public:
		MenuEntryCfgFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<MenuEntryCfgFactory, T>(_key) {};
	};

}
