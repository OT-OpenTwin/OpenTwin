//! @file EntityHandler.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

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
	void addBackgroundImage(const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter, ot::NewModelStateInfo& _newEntities);
public:
	void addBackgroundImages(const std::list<std::string>& _fileNames, const std::list<std::string>& _fileContent, const std::list<int64_t>& _uncompressedDataLength, const std::string& _fileFilter);

	bool addImageToProject(const std::string& _projectEntityName, const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter);
	bool removeImageFromProjects(const std::list<ot::EntityInformation>& _projects);

	void addContainer();

	ot::ReturnMessage updateDocumentText(EntityBlockHierarchicalDocumentItem* _documentItem, const std::string& _content);

private:
	bool getFileFormat(const std::string& _filePath, std::string& _fileName, std::string& _extensionString, ot::FileExtension::DefaultFileExtension& _extension) const;
	bool getImageFileFormat(const std::string& _filePath, std::string& _fileName, std::string& _extension, ot::ImageFileFormat& _format) const;
	
	const std::string c_rootFolderPath;
	const std::string c_projectsFolder;
	const std::string c_projectsFolderName = "Projects";
	const std::string c_documentsFolder;
	const std::string c_documentsFolderName = "Documents";
	const std::string c_containerFolder;
	const std::string c_containerFolderName = "Container";
	const std::string c_backgroundFolder;
	const std::string c_backgroundFolderName = "Background";
	const std::string c_connectionsFolder;
	const std::string c_connectionsFolderName = "Connections";

};