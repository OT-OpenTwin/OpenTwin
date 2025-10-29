// @otlicense

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
