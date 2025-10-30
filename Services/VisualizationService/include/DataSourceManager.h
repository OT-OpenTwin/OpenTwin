// @otlicense
// File: DataSourceManager.h
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

class DataSourceManagerItem;

#include <string>
#include <map>

#include "OldTreeIcon.h"

#include "OTServiceFoundation/ModelComponent.h"

class DataSourceManager
{
public:
	DataSourceManager();
	virtual ~DataSourceManager();

	static DataSourceManagerItem *getDataItem(ot::UID sourceID, ot::UID sourceVersion, ot::UID meshID, ot::UID meshVersion, ot::components::ModelComponent *modelComponent);

	static void deleteDataItem(ot::UID sourceID, ot::UID sourceVersion);
	static bool isDataItemLoaded(ot::UID sourceID, ot::UID sourceVersion);

	static void deleteAllDataItems(void);

private:
	static std::map<std::pair<ot::UID, ot::UID>, DataSourceManagerItem *> dataItems;
};
