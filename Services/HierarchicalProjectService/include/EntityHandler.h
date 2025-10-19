//! @file EntityHandler.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Open twin header
#include "OTCore/CoreTypes.h"
#include "OTCore/FolderNames.h"
#include "OTCore/ProjectInformation.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "NewModelStateInfo.h"
#include "EntityBlockHierarchicalProjectItem.h"

class EntityHandler : public BusinessLogicHandler {
public:
	EntityHandler();
	virtual ~EntityHandler();

	std::shared_ptr<EntityBlockHierarchicalProjectItem> createProjectItemBlockEntity(const ot::ProjectInformation& _projectInfo, const ot::EntityInformation& _parentEntity);

	bool addConnection(const ot::GraphicsConnectionCfg& _connection);

	void addDocument(const ot::EntityInformation& _containerInfo, const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter, ot::NewModelStateInfo& _newEntities);
	void addDocuments(const ot::EntityInformation& _containerInfo, const std::list<std::string>& _fileNames, const std::list<std::string>& _fileContent, const std::list<int64_t>& _uncompressedDataLength, const std::string& _fileFilter);

	void addBackgroundImage(const ot::EntityInformation& _containerInfo, const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter, ot::NewModelStateInfo& _newEntities);
	void addBackgroundImages(const ot::EntityInformation& _containerInfo, const std::list<std::string>& _fileNames, const std::list<std::string>& _fileContent, const std::list<int64_t>& _uncompressedDataLength, const std::string& _fileFilter);

	bool addImageToProject(const std::string& _projectEntityName, const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter);
	bool removeImageFromProjects(const std::list<ot::EntityInformation>& _projects);

	void addContainer(const ot::EntityInformation& _containerInfo);

private:
	bool getImageFileFormat(const std::string& _filePath, std::string& _fileName, std::string& _extension, ot::ImageFileFormat& _format);

	const std::string c_connectionsFolder = "Connections";

};