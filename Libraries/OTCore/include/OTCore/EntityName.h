//! @file String.h
//! @authors Alexander Kuester (alexk95), Jan Wagner (JWagner)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>
#include <optional>

namespace ot {

	class OT_CORE_API_EXPORT EntityName {
		OT_DECL_STATICONLY(EntityName)
	public:
		//! @brief Extract a name from the full topology entity name.
		//! @param _topologyLevel -1 = Entity name, 0 = root folder, 1 = next level container .... n = short name of entity (default).
		//! @return Empty optional if the topology is out of range.
		static std::optional<std::string> getSubName(const std::string& _fullEntityName, int32_t _topologyLevel = -1);
		
		//! @brief Returns true if the child entity is a child of the parent entity.
		//! The child entity may be further nested.
		//! @param _childEntityPath Full path of the child entity
		//! @param _parentEntityPath Full path of the parent entity
		static bool isChildOf(const std::string& _childEntityPath, const std::string& _parentEntityPath);

		//! @brief Returns the topology level of the entity.
		//! The topology level of the root entity is 0. For the entity "Root/Parent/Entity" a topology level of 2 will be returned.
		//! @param _entityPath Full path of the entity.
		static size_t getTopologyLevel(const std::string& _entityPath);

		//! @brief Changes the parent path of an entity to the new path.
		//! This will only change the topology path up to the topology level of the new parent path.
		//! 
		//! If the current entity path is "Root/Parent/Entity" and the new parent path is "NewRoot/NewParent" the result will be "NewRoot/NewParent/Entity".
		//! 
		//! If the current entity path is "Root/Parent/Entity" and the new parent path is "NewParent" the result will be "NewParent/Parent/Entity".
		//! @warning The topology level of the new parent path must be less than the topology level of the current entity path.
		//! @param _currentEntityPath The full path of the entity.
		//! @param _newParentPath The new parent path of the entity.
		//! @throw Throws ot::OutOfBoundsException if the topology level of the new parent path is not smaller than the one of the entity.
		static std::string changeParentPath(const std::string& _currentEntityPath, const std::string& _newParentPath);

		//! @brief Changes the entity sub name by adding it as a child to the parent path.
		//! The method will reparent the leaf of the entity path to the provided parent entity path.
		//! 
		//! If the current entity path is "Root/Parent/Entity" and the new parent path is "NewRoot/NewParent" the result will be "NewRoot/NewParent/Entity".
		//! 
		//! If the current entity path is "Root/Parent/Entity" and the new parent path is "NewParent" the result will be "NewParent/Entity".
		//! @param _currentEntityPath The full path of the entity.
		//! @param _newParentPath The new parent path of the entity.
		static std::string changeParentWithTopo(const std::string& _currentEntityPath, const std::string& _newParentPath);
	};
}