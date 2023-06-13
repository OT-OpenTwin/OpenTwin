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
	void AddSourceFileToModel();
	void StorePythonScriptAsEntity(std::string fileName);

	void ReserveSourceUIDs(const ot::UIDList& entityID) { _reserveSourceUIDs = entityID; };
	std::vector<char> ExtractFileContentAsBinary(std::string fileName);

private:
	ot::UIDList _reserveSourceUIDs;
	const std::string _dataSourceFolder;
	std::shared_ptr<EntityParameterizedDataSource> CreateNewSourceEntity(std::string dataType);
};
