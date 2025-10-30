// @otlicense
// File: ProjectToCollectionConverter.cpp
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

#include "ProjectToCollectionConverter.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/JSON.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/ProjectInformation.h"


ProjectToCollectionConverter::ProjectToCollectionConverter(const std::string& _sessionServiceURL)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetAuthorisationServerUrl, doc.GetAllocator()), doc.GetAllocator());
	std::string responseStr;
	if (!ot::msg::send("", _sessionServiceURL, ot::EXECUTE, doc.toJson(), responseStr)) {
		throw std::exception("Could not get authorisation service URL from session service.");
	}
	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (!response.isOk()) {
		throw std::exception(("Could not get authorisation service URL from session service due to error: " + response.getWhat()).c_str());
	}

	m_authorisationService = response.getWhat();
}

std::string ProjectToCollectionConverter::nameCorrespondingCollection(const std::string& projectName, const std::string& userName, const std::string& userPSW)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(userPSW, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	
	ot::msg::send("", m_authorisationService, ot::EXECUTE, doc.toJson(), response);
	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);

	if (!responseMessage.isOk()) {
		throw std::exception(std::string("Could not get information about project \"" + projectName + "\".").c_str());
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());

	ot::ProjectInformation info(responseDoc.getConstObject());
	return info.getCollectionName();
}
