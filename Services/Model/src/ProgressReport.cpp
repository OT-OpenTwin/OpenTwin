// @otlicense
// File: ProgressReport.cpp
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

#include "stdafx.h"

#include "Application.h"
#include "ProgressReport.h"

#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

std::mutex ProgressReportLockGuard;

void ProgressReport::setProgressInformation(std::string message, bool continuous)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SetProgressVisibility, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(message, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_VisibleState, true, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContinuousState, continuous, doc.GetAllocator());

	std::string response;
	Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, response);
}

void ProgressReport::setProgress(int percentage)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SetProgressbarValue, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PERCENT, percentage, doc.GetAllocator());

	std::string response;
	Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, response);
}

void ProgressReport::closeProgressInformation(void)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SetProgressVisibility, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString("", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_VisibleState, false, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContinuousState, false, doc.GetAllocator());

	std::string response;
	Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, response);
}
