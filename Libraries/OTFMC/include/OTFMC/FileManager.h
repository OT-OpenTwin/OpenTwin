// @otlicense
// File: FileManager.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTFMC/FMCache.h"
#include "OTFMC/FMNewProjectInfo.h"

// std header
#include <mutex>

namespace ot {

	class FileManager : public ActionHandler {
		OT_DECL_NOCOPY(FileManager)
		OT_DECL_NOMOVE(FileManager)
	public:
		FileManager();
		~FileManager();

	private:

		FMCache m_cache;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Action handler

	private:
		ReturnMessage initializeNewProject();
		ReturnMessage commitData(JsonDocument& _doc);
		ReturnMessage checkoutData(JsonDocument& _doc);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Workers

		void workerInitializeNewProject(FMNewProjectInfo&& _newProjectInfo);


	};

}