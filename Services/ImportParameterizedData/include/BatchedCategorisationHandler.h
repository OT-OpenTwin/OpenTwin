// @otlicense
// File: BatchedCategorisationHandler.h
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
#include "OTServiceFoundation/PythonServiceInterface.h"
#include "EntityTableSelectedRanges.h"
#include "BusinessLogicHandler.h"
#include "BatchUpdateInformation.h"
#include "UniqueEntityNameCreator.h"

#include <memory>
#include <list>

class BatchedCategorisationHandler : public BusinessLogicHandler
{
public:
	BatchedCategorisationHandler() = default;
	~BatchedCategorisationHandler();
	void createNewScriptDescribedMSMD(std::list<ot::UID> _selectedEntities);
	void addCreator();
	
	

private:
	ot::PythonServiceInterface* m_pythonInterface = nullptr;
	std::string m_rmdEntityName;
	ot::UID m_scriptFolderUID = -1;
	UniqueEntityNameCreator m_uniqueEntityNameCreator;
	std::list<std::shared_ptr<EntityTableSelectedRanges>> m_consideredRanges;

	std::list<std::shared_ptr<EntityTableSelectedRanges>> findAllTableSelectionsWithConsiderForBatching();
	void run(bool _lastRun, std::map<std::string, ot::UID>& _tableUIDsByName, std::map<std::string, ot::UID>& _pythonScriptsByName);

	inline void ensureEssentials();
	std::map<uint32_t, std::list<BatchUpdateInformation>> createNewMSMDWithSelections(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD, bool _lastRun, std::map<std::string, ot::UID>& _tableUIDsByName, std::map<std::string, ot::UID>& _pythonScriptsByName);
	std::map<std::string, ot::UID> getAllTableUIDsByName();
	std::map<std::string, ot::UID> getAllPythonScriptUIDsByName();
};
