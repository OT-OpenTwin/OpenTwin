// @otlicense
// File: UIControlsHandler.h
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
#include "Application.h"

#include "OTGui/ToolBarButtonCfg.h"
#include <atomic>

class UIControlsHandler
{
public:
	UIControlsHandler();
	
	const std::atomic_bool& getUICreated() { return m_uiCreated; }

private:
	std::atomic_bool m_uiCreated = false;

	const std::string m_pageModel = "Model";

	const std::string m_groupGeometry = "Geometry";
	
	const std::string m_groupParameters = "Parameters";
	const std::string m_groupEdit = "Edit";
	const std::string m_groupImport = "File Imports";
	
	ot::ToolBarButtonCfg m_buttonGeometryInfo;

	ot::ToolBarButtonCfg m_buttonCreateParameter;
	
	ot::ToolBarButtonCfg m_buttonRedo;
	ot::ToolBarButtonCfg m_buttonUndo;
	ot::ToolBarButtonCfg m_buttonDelete;
	
	void createPages();
	void createGroups();
	void setupButtons();
	void createButtons();
	
};
