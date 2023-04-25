/*
 * ProjectManagement.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>

class AppBase;

class ProjectManagement
{
public:
	ProjectManagement();
	~ProjectManagement();
	
	void setDataBaseURL(const std::string &url);
	void setAuthServerURL(const std::string &url);

	bool InitializeConnection(void);

	bool createProject(const std::string &projectName, const std::string &userName, const std::string &defaultSettingTemplate);
	bool deleteProject(const std::string &projectName);
	std::string exportProject(const std::string &projectName, const std::string &exportFileName, AppBase *parent);
	std::string importProject(const std::string &projectName, const std::string &userName, const std::string &importFileName, AppBase *parent);
	bool renameProject(const std::string &oldProjectName, const std::string &newProjectName);
	bool projectExists(const std::string &projectName, bool &canBeDeleted);
	std::string getProjectCollection(const std::string &projectName);
	bool findProjectNames(const std::string &projectNameFilter, int maxNumberOfResults, std::list<std::string> &projectsFound, bool &maxLengthExceeded);
	bool getProjectAuthor(const std::string &projectName, std::string &author);
	bool readProjectAuthor(std::list<std::string> &projects);
	bool copyProject(const std::string &sourceProjectName, const std::string &destinationProjectName, const std::string &userName);
	std::vector<std::string> ProjectManagement::getDefaultTemplateList(void);
	bool canAccessProject(const std::string &projectCollection);

private:
	bool createNewCollection(const std::string &collectionName, const std::string &defaultSettingTemplate);
	bool hasError(const std::string &response);
	bool hasSuccessful(const std::string &response);

	bool isConnected;
	std::string databaseURL;
	std::string authServerURL;
	const std::string dataBaseName;
	const std::string projectCatalogCollectionName;
	std::map<std::string, std::string> projectAuthorMap;
};
