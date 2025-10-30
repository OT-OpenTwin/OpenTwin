// @otlicense
// File: EntityParameterizedDataTable.h
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

#include "EntityResultTable.h"

#include <string>
#include <stdint.h>
#include <vector>
#include <map>
#include "OTGui/TableCfg.h"

class __declspec(dllexport) EntityParameterizedDataTable : public EntityResultTable<std::string>
{
public:
	EntityParameterizedDataTable() : EntityParameterizedDataTable(0, nullptr, nullptr, nullptr, "") {};
	EntityParameterizedDataTable(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, const std::string &owner);
	void SetTableDimensions(uint32_t numberOfRows, uint32_t numberOfColumns);
	void SetSourceFile(std::string sourceFileName, std::string sourceFilePath);
	virtual std::string getClassName(void) const override { return "EntityParameterizedDataTable"; };
	std::string getFileName() const { return _sourceFileName; }
	std::string getFilePath() const { return _sourceFilePath; }

	void createProperties(ot::TableCfg::TableHeaderMode _defaultHeaderMode);
	std::string getSelectedHeaderModeString();
	ot::TableCfg::TableHeaderMode getSelectedHeaderMode();
	
private:
	void createProperties() override {};
	uint32_t _numberOfColumns = 0;
	uint32_t _numberOfRows = 0;
	std::vector<uint32_t> _numberOfUniquesInColumns;
	std::string _sourceFileName;
	std::string _sourceFilePath;
	const std::string _defaulCategory = "tableInformation";

	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;


};
