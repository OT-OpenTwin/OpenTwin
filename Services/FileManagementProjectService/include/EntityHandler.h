// @otlicense
// File: EntityHandler.h
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

// Open twin header
#include "OTCore/CoreTypes.h"
#include "OTCore/FolderNames.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/ProjectInformation.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "NewModelStateInfo.h"

class EntityHandler : public BusinessLogicHandler {
public:
	EntityHandler() = delete;
	EntityHandler(const std::string& _rootFolderPath);
	virtual ~EntityHandler();

};