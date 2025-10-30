// @otlicense
// File: ConfigurationHelper.h
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
#include <string>
#include "OTCore/EntityName.h"
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"

namespace ot
{
	namespace ConfigurationHelper
	{
		static std::string getGraphicSceneName(const std::string& _entityName, const std::string& _graphicsSceneChildName)
		{
			std::string packageName = "";
			if (_graphicsSceneChildName != "")
			{

				// Would fail if the item name is the same as the container: root/data/data
				//size_t numberOfFindings = ot::String::count(_entityName, _graphicsSceneChildName);
				//if (numberOfFindings != 1)
					//{
					//OT_LOG_E("Failed to determine the name of the associated graphics scene.");
					//}
				//else
					//{
					size_t childOfGraphicsScenePos = _entityName.find(_graphicsSceneChildName);
					packageName = _entityName.substr(0, childOfGraphicsScenePos - 1);
				//}
			}
			else
			{
				packageName = ot::EntityName::getParentPath(_entityName);
			}

			return packageName;
		}


	}
}
