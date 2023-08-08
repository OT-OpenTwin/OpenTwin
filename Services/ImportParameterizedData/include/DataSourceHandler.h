/*****************************************************************//**
 * \file   DataSourceHandler.h
 * \brief  Central class for dealing with source files.
 * 
 * \author Wagner
 * \date   April 2023
 *********************************************************************/
#pragma once
#include <string>
#include <memory>

#include "EntityFile.h"
#include "BusinessLogicHandler.h"

class DataSourceHandler : public BusinessLogicHandler
{
public:
	void AddNewFilesToModel(ot::UIDList& topologyIDs, ot::UIDList& topologyVersions, ot::UIDList& dataIDs, ot::UIDList& dataVersions, std::list<std::string>& fileNames);
	
private:
	
};
