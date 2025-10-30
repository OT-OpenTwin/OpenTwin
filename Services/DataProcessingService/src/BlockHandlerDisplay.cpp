// @otlicense
// File: BlockHandlerDisplay.cpp
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

#include "BlockHandlerDisplay.h"
#include "StringConverter.h"
#include "OTCore/String.h"
#include "SolverReport.h"

BlockHandlerDisplay::BlockHandlerDisplay(EntityBlockDisplay* _blockEntity, const HandlerMap& _handlerMap)
	:BlockHandler(_blockEntity ,_handlerMap)
{
	m_input = _blockEntity->getConnectorInput();
	m_description = _blockEntity->getDescription();
}

bool BlockHandlerDisplay::executeSpecialized()
{
	std::string displayMessage = m_description + "\n\n";

	auto incomming = m_dataPerPort.find(m_input.getConnectorName());
	if (incomming->second != nullptr)
	{
		ot::JsonValue& data = incomming->second->getData();
		const std::string dataPretty = ot::json::toPrettyString(data);
		
		displayMessage += "Data: \n" + dataPretty;

		
		ot::JsonValue& metadata =incomming->second->getMetadata();
		const std::string metaDataPretty = ot::json::toPrettyString(metadata);
		displayMessage += "\nMetadata:\n" + metaDataPretty;
		_uiComponent->displayMessage(displayMessage + "\n");
	}
	else
	{
		_uiComponent->displayMessage(getErrorDataPipelineNllptr() + "\n");
	}
	return true;
}

std::string BlockHandlerDisplay::getBlockType() const
{
	return "Display Block";
}
