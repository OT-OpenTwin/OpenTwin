// @otlicense
// File: BlockHandlerPython.h
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
#include "EntityBlockPython.h"
#include "OTServiceFoundation/PythonServiceInterface.h"

class BlockHandlerPython : public BlockHandler
{
public: 
	BlockHandlerPython(EntityBlockPython* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

	// Inherited via BlockHandler
	std::string getBlockType() const override;

private:
	std::list<std::string> m_requiredInput;
	std::list<std::string> m_outputs;
	std::list<PipelineData> m_outputData;
	std::string m_scriptName;
	std::string m_entityName;
	ot::UID m_manifestUID = ot::invalidUID;
	ot::PythonServiceInterface* m_pythonServiceInterface = nullptr;

};
