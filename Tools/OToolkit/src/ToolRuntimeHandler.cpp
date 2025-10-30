// @otlicense
// File: ToolRuntimeHandler.cpp
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

// Toolkit header
#include "ToolManager.h"
#include "ToolRuntimeHandler.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"

ToolRuntimeHandler::ToolRuntimeHandler(otoolkit::Tool* _tool, ToolManager* _manager) 
	: m_tool(_tool), m_manager(_manager)
{
	OTAssertNullptr(m_manager);
}

ToolRuntimeHandler::~ToolRuntimeHandler() {
	
}

void ToolRuntimeHandler::notifyDataChanged(void) {
	OTAssertNullptr(m_manager);
	m_manager->toolDataHasChanged(this);
}