// @otlicense
// File: EntityHandler.h
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

// Open twin header
#include "OTCore/CoreTypes.h"
#include "OTCore/FolderNames.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/ProjectInformation.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "NewModelStateInfo.h"
#include "EntityBlockHierarchicalProjectItem.h"
#include "EntityBlockHierarchicalDocumentItem.h"

class EntityHandler : public BusinessLogicHandler {
public:
	EntityHandler() = delete;
	EntityHandler(const std::string& _rootFolderPath);
	virtual ~EntityHandler();

	void createProjectItemBlockEntity(const ot::ProjectInformation& _projectInfo);

	bool addConnection(const ot::GraphicsConnectionCfg& _connection);

private:
	void addDocument(const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter, ot::NewModelStateInfo& _newEntities);
public:
	void addDocuments(const std::list<std::string>& _fileNames, const std::list<std::string>& _fileContent, const std::list<int64_t>& _uncompressedDataLength, const std::string& _fileFilter);

private:
	void addImage(const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter, ot::NewModelStateInfo& _newEntities);
public:
	void addImages(const std::list<std::string>& _fileNames, const std::list<std::string>& _fileContent, const std::list<int64_t>& _uncompressedDataLength, const std::string& _fileFilter);

	void addLabel();

private:
	void updateProjectImage(const ot::EntityInformation& _projectInfo, ot::NewModelStateInfo& _newEntities, ot::NewModelStateInfo& _updateEntities, std::list<ot::UID>& _removalEntities);
public:
	void updateProjectImages(const std::list<ot::EntityInformation>& _projects);

	bool addImageToProject(const std::string& _projectEntityName, const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter);
	bool removeImageFromProjects(const std::list<ot::EntityInformation>& _projects);

	void addContainer();

private:
	bool getFileFormat(const std::string& _filePath, std::string& _fileName, std::string& _extensionString, ot::FileExtension::DefaultFileExtension& _extension) const;
	bool getImageFileFormat(const std::string& _filePath, std::string& _fileName, std::string& _extension, ot::ImageFileFormat& _format) const;
	
	const std::string c_rootFolderPath;
	const std::string c_projectsFolder;
	const std::string c_projectsFolderName;
	const std::string c_documentsFolder;
	const std::string c_documentsFolderName;
	const std::string c_containerFolder;
	const std::string c_containerFolderName;
	const std::string c_decorationFolder;
	const std::string c_decorationFolderName;
	const std::string c_connectionsFolder;
	const std::string c_connectionsFolderName;

};