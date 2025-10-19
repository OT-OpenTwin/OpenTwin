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
#include "EntityBlockHierarchicalProjectItem.h"

class EntityHandler : public BusinessLogicHandler {
public:
	EntityHandler();
	virtual ~EntityHandler();

	std::shared_ptr<EntityBlockHierarchicalProjectItem> createProjectItemBlockEntity(const ot::ProjectInformation& _projectInfo, const ot::EntityInformation& _parentEntity);

	bool addConnection(const ot::GraphicsConnectionCfg& _connection);

	bool addImageToProject(const std::string& _projectEntityName, const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter);
	bool removeImageFromProjects(const std::list<ot::EntityInformation>& _projects);

private:
	const std::string c_connectionsFolder = "Connections";

};