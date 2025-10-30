// @otlicense
// File: UserSettings.cpp
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

#include "UserSettings.h"
#include "AppBase.h"

#include "OTSystem/OTAssert.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/ApplicationPropertiesManager.h"

UserSettings& UserSettings::instance(void) {
	static UserSettings g_instance;
	return g_instance;
}

void UserSettings::showDialog(void) {
	ot::ApplicationPropertiesManager::instance().showDialog();
}

void UserSettings::showDialog(const QString& _group) {
	ot::ApplicationPropertiesManager::instance().showDialog();
}

void UserSettings::clear(void) {
	ot::ApplicationPropertiesManager::instance().clear();
}

void UserSettings::addSettings(const std::string& _serviceName, const ot::PropertyGridCfg& _config) {
	ot::ApplicationPropertiesManager::instance().add(_serviceName, _config);
}

// #######################################################################################

// Slots

void UserSettings::slotItemChanged(const std::string& _owner, const ot::Property* _property) {
	AppBase::instance()->settingsChanged(_owner, _property);
}

// #######################################################################################

// Private member

UserSettings::UserSettings() {
	this->connect(&ot::ApplicationPropertiesManager::instance(), &ot::ApplicationPropertiesManager::propertyChanged, this, &UserSettings::slotItemChanged);
}
