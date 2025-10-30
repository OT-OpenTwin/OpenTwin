// @otlicense
// File: ModalCommandBase.h
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
#include "OTServiceFoundation/FoundationAPIExport.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

// std header
#include <list>
#include <string>
#include <map>
#include "OldTreeIcon.h"

namespace ot {

	class OT_SERVICEFOUNDATION_API_EXPORT ModalCommandBase
	{
	public:
		ModalCommandBase(ot::ApplicationBase *app, const std::string &actionMnuID, const std::string &actionBtnID);
		virtual ~ModalCommandBase();

		virtual bool executeAction(const std::string &action, rapidjson::Document &doc) = 0;

		virtual void modelSelectionChanged(std::list< UID> &selectedEntityID) = 0;

	protected:
		virtual std::string initializeAndCreateUI(const LockTypes& modelRead, const LockTypes& modelWrite) = 0;

		void setupUI(void);
		void addMenuPage(const std::string &menu);
		void addMenuGroup(const std::string &menu, const std::string &group);
		void addMenuSubgroup(const std::string &menu, const std::string &group, const std::string &subgroup);
		void addMenuAction(const std::string &menu, const std::string &group, const std::string &buttonName, const std::string &text, const LockTypes& flags, const std::string &iconName, const std::string &iconFolder = std::string("Default"));
		void addMenuAction(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &buttonName, const std::string &text, const LockTypes& flags, const std::string &iconName, const std::string &iconFolder = std::string("Default"));
		void addMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, const std::string &boxText, bool checked, const LockTypes& flags);
		void addMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, const std::string &editLabel, const LockTypes& flags);
		void setMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, bool checked);
		void setMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, bool error);

	protected:
		ot::ApplicationBase *application;

	private:
		std::string actionMenuID;
		std::string actionButtonID;

		std::map<std::string, bool>    uiMenuMap;
		std::map<std::string, bool>    uiGroupMap;
		std::map<std::string, bool>    uiSubGroupMap;
		std::map<std::string, bool>    uiActionMap;
	};
}