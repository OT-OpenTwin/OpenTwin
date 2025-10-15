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

	std::shared_ptr<EntityBlockHierarchicalProjectItem> createProjectItemBlockEntity(const ot::ProjectInformation& _projectInfo, const std::string& _parentEntity);



private:

};