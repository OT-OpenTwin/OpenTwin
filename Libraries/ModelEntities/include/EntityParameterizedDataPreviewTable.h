// @otlicense
// File: EntityParameterizedDataPreviewTable.h
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
#pragma once

#include "EntityResultTable.h"

#include <string>
#include <stdint.h>
#include <vector>
#include <map>

class __declspec(dllexport) EntityParameterizedDataPreviewTable : public EntityResultTable<std::string>
{
public:
	EntityParameterizedDataPreviewTable() : EntityParameterizedDataPreviewTable(0, nullptr, nullptr, nullptr) {};
	EntityParameterizedDataPreviewTable(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms);
	virtual std::string getClassName(void) const override { return "EntityParameterizedDataPreviewTable"; };
	
	std::vector<std::pair<ot::UID, ot::UID>> GetLastStatusOfPreview() { return _displayedRanges; };
	void AddRangeToPreviewStatus(std::pair<ot::UID, ot::UID> range) { _displayedRanges.push_back(range); }

private:
	std::vector<std::pair<ot::UID, ot::UID>> _displayedRanges;


	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;


};
