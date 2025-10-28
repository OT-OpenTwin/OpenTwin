/*
 * ProjectManagement.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// Frontend header
#include "LoginData.h"

// OpenTwin Core header
#include "OTCore/ProjectInformation.h"

// std header
#include <map>
#include <list>
#include <string>
#include <vector>

// BSON header
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>

class AppBase;

class ProjectManagement {
public:
	ProjectManagement();
	ProjectManagement(const LoginData& _data);
	~ProjectManagement();
	
	void setDataBaseURL(const std::string &url);
	void setAuthServerURL(const std::string &url);

	bool InitializeConnection(void);

	bool createProject(const std::string &projectName, const std::string& projectType, const std::string &userName, const std::string &defaultSettingTemplate);
	bool deleteProject(const std::string &projectName);
	std::string exportProject(const std::string &projectName, const std::string &exportFileName, AppBase *parent);
	std::string importProject(const std::string &projectName, const std::string &userName, const std::string &importFileName, AppBase *parent);
	bool renameProject(const std::string &oldProjectName, const std::string &newProjectName);
	bool projectExists(const std::string &projectName, bool &canBeDeleted);
	std::string getProjectCollection(const std::string &projectName);
	bool findProjects(const std::string& _projectNameFilter, int _maxNumberOfResults, std::list<ot::ProjectInformation>& _projectsFound, bool& _maxLengthExceeded);
	ot::ProjectInformation getProjectInformation(const std::string& _projectName);
	bool readProjectsInfo(std::list<std::string>& _projects);
	bool copyProject(const std::string &sourceProjectName, const std::string &destinationProjectName, const std::string &userName);
	std::vector<std::string> getDefaultTemplateList(void);
	bool canAccessProject(const std::string &projectCollection);

	std::string getProjectType(const std::string& projectName);

	bool updateAdditionalInformation(const ot::ProjectInformation& projectInfo);

private:
	bool createNewCollection(const std::string &collectionName, const std::string &defaultSettingTemplate);
	bool hasError(const std::string &response);
	bool hasSuccessful(const std::string &response);
	void copyCollection(const std::string& sourceCollectionName, const std::string& destinationCollectionName);
	void flushCachedDocuments(std::list<bsoncxx::builder::basic::document*>& cachedDocuments, mongocxx::collection& collection);

	bool m_isConnected;
	std::string m_databaseURL;
	std::string m_authServerURL;
	const std::string m_dataBaseName;
	const std::string m_projectCatalogCollectionName;
	std::map<std::string, ot::ProjectInformation> m_projectInfoMap;
};
