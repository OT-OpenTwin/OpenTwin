// @otlicense
// File: ContextMenuManagerHandler.h
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
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/MenuRequestEvent.h"
#include "OTWidgets/Menu/Menu.h"

namespace ot {

	class MenuManager;
	class MenuCallbackBase;

	class OT_WIDGETS_API_EXPORT MenuManagerHandler
	{
	public:
		MenuManagerHandler();
		virtual ~MenuManagerHandler() = default;

		virtual BasicEntityInformation getEntityInformationFromName(const std::string& _entityName, bool* _failFlag = (bool*)nullptr) const = 0;
		virtual MenuCfg getMenuConfiguration(const MenuCallbackBase* _callbackObject, const MenuRequestEvent& _requestEvent) const = 0;
		virtual bool notifyMenuOwner(const MenuAction* _action) const = 0;
		virtual bool triggerToolbarButton(const std::string& _buttonName) const = 0;
		virtual bool checkToolBarButtonExists(const std::string& _buttonName) const = 0;
		virtual bool isToolBarButtonEnabled(const std::string& _buttonName) const = 0;
	};

}