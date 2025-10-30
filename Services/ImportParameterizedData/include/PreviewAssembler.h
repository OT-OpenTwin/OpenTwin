// @otlicense
// File: PreviewAssembler.h
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
#include "EntityParameterizedDataTable.h"
#include "EntityParameterizedDataPreviewTable.h"
#include "EntityTableSelectedRanges.h"
#include "EntityResultTableData.h"
#include "OTServiceFoundation/ModelComponent.h"

#include <memory>
#include <string>
#include <list>
#include <vector>

class PreviewAssembler
{
public:
	PreviewAssembler(ot::components::ModelComponent* modelComponent, std::string tableFolder) : _modelComponent(modelComponent), _tableFolder(tableFolder) {};
	std::shared_ptr<EntityParameterizedDataPreviewTable> AssembleTable(ot::UIDList& existingRanges);

protected:
	int _numberOfFields = 0;
	std::vector<std::shared_ptr<EntityTableSelectedRanges>> _selectedRangeEntities;
	std::vector<std::shared_ptr<EntityParameterizedDataTable>> _tableSources;

	virtual void InitiatePreviewTable(std::shared_ptr<EntityResultTableData<std::string>> sourceTable) = 0;
	virtual void AddFieldsToTable(std::shared_ptr<EntityResultTableData<std::string>> previewTable) = 0;

private:
	ot::components::ModelComponent* _modelComponent = nullptr;
	std::string _tableFolder;
	void CountPreviewFieldsFromSelectedRanges();

	void LoadSelectedRangesAndTableSources(ot::UIDList& tableNames);
	void LoadTableSources(std::list<std::string>& tableNames);

};
