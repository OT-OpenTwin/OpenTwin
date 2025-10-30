// @otlicense
// File: BlockHandlerStorage.h
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
#include "BlockHandler.h"
#include "EntityBlockStorage.h"
#include "ResultCollectionExtender.h"
#include "MetadataParameter.h"

class BlockHandlerStorage : public BlockHandler
{
public:
	BlockHandlerStorage(EntityBlockStorage* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerStorage() = default;

	bool executeSpecialized() override;

	// Inherited via BlockHandler
	std::string getBlockType() const override;
private:
	EntityBlockStorage* m_blockEntityStorage;
	
	std::list<std::string> m_allDataInputs;
	std::string m_seriesMetadataInput;
	bool m_createPlot;
	std::string m_plotName;


	bool allInputsAvailable();
	std::list<DatasetDescription> createDatasets();


};
