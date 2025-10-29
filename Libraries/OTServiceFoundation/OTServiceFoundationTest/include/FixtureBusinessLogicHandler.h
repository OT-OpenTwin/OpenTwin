// @otlicense

#pragma once
#include <gtest/gtest.h>
#include "OTServiceFoundation/BusinessLogicHandler.h"

class FixtureBusinessLogicHandler : public testing::Test
{
public:
	std::vector<std::string> SetAnother5Names(std::list<std::string>& alreadySetNames)
	{
		return _handler.CreateNewUniqueTopologyNames(alreadySetNames, _folderName, _fileName, 5);
	}

	std::vector<std::string> SetAnother5NamesStartWith7(std::list<std::string>& alreadySetNames)
	{
		return _handler.CreateNewUniqueTopologyNames(alreadySetNames, _folderName, _fileName, 5,7);
	}

	std::vector<std::string> SetAnother5NamesAlwaysNumbered(std::list<std::string>& alreadySetNames)
	{
		return _handler.CreateNewUniqueTopologyNames(alreadySetNames, _folderName, _fileName, 5, 1,true);
	}


	std::string getFileName()
	{
		return "File";
	} 
	std::string getFolderName() 
	{
		return "Folder";
	}
private:
	BusinessLogicHandler _handler;
	const std::string _folderName = "Folder";
	const std::string _fileName = "File";

};
