#include <fstream>
#include <exception>
#include <vector>

#include <OpenTwinCommunication/ActionTypes.h>

#include "DataSourceHandler.h"

#include "EntityFileCSV.h"
#include "EntityBinaryData.h"
#include "ClassFactory.h"


void DataSourceHandler::AddNewFilesToModel(ot::UIDList& topologyIDs, ot::UIDList& topologyVersions, ot::UIDList& dataIDs, ot::UIDList& dataVersions, std::list<std::string>& fileNames)
{
	std::list<bool> forceVis;
	for (uint64_t i = 0; i < topologyIDs.size(); i++)
	{
		forceVis.push_back(false);
	}
	_modelComponent->addEntitiesToModel(topologyIDs, topologyVersions, forceVis, dataIDs, dataVersions, topologyIDs, "added new files");
	std::string displayMessage = "Loaded files: \n";
	for (const std::string& fileName : fileNames)
	{
		displayMessage.append(fileName);
		displayMessage.append("\n");
	}
	_uiComponent->displayMessage(displayMessage);
}

