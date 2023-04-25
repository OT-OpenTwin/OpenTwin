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

#include "EntityParameterizedDataSource.h"
#include "BusinessLogicHandler.h"

class DataSourceHandler : public BusinessLogicHandler
{
public:
	DataSourceHandler(const std::string dataSourceFolder);
	void StoreSourceFileAsEntity(std::string fileName);
	std::vector<char> ExtractFileContentAsBinary(std::string fileName);
private:
	
	const std::string _dataSourceFolder;
	std::shared_ptr<EntityParameterizedDataSource> CreateNewSourceEntity(std::string dataType);
};
