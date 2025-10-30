// @otlicense
// File: UserSettings.h
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

#include "OTCore/JSON.h"
#include "OTGui/PropertyGridCfg.h"

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include <map>
#include <list>

namespace ot { class Property; }

class UserSettings : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(UserSettings)
public:
	static UserSettings& instance(void);

	void showDialog(void);

	void showDialog(const QString& _group);

	// #######################################################################################

	// Data management

	void clear(void);

	void addSettings(const std::string& _serviceName, const ot::PropertyGridCfg& _config);

	// #######################################################################################

private Q_SLOTS:
	void slotItemChanged(const std::string& _owner, const ot::Property* _property);

private:
	UserSettings();

};
