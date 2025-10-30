// @otlicense
// File: ProjectInformationHandler.h
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
#include "OTCommunication/ActionHandler.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/UiComponent.h"

class ProjectInformationHandler : public ot::ActionHandler, public ot::ButtonHandler {
	OT_DECL_NOCOPY(ProjectInformationHandler)
	OT_DECL_NOMOVE(ProjectInformationHandler)
public:
	ProjectInformationHandler(ot::ActionDispatcher* _dispatcher = &ot::ActionDispatcher::instance());
	~ProjectInformationHandler();

	void addButtons(ot::components::UiComponent* _ui);

	// ##################################################################################################################################################################################################################

	// Action and Button handler

private:
	ot::ReturnMessage applyProjectInformation(ot::JsonDocument& _document);
	void requestUploadProjectPreviewImage();

	ot::ToolBarButtonCfg m_editButton;
};