// @otlicense
// File: PropertyBundleEventHandling.h
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
#include "OTModelEntities/Properties/Bundle/PropertyBundle.h"
class PropertyBundleEventHandling : public PropertyBundle
{
public:
	void setFolderIDScripts(ot::UID _scriptFolderID) { m_scriptFolderID = _scriptFolderID; }
	void setFolderIDManifests(ot::UID _manifestFolderID) { m_manifestFolderID = _manifestFolderID; }
	void setProperties(EntityBase* _thisObject) override;
	bool updatePropertyVisibility(EntityBase* _thisObject) override;

	std::pair<ot::UID, std::string> getSelectedScript(EntityBase* _thisObject);
	std::pair<ot::UID, std::string> getSelectedManifest(EntityBase* _thisObject);
private:
	const std::string m_group = "Event execution";
	const std::string m_propertyScript = "Script";
	const std::string m_propertyEnvironment = "Environment";
	ot::UID m_scriptFolderID = ot::invalidUID;
	ot::UID m_manifestFolderID = ot::invalidUID;
};
