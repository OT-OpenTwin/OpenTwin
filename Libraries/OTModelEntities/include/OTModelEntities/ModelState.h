// @otlicense
// File: ModelState.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/VersionGraphCfg.h"
#include "OTDataStorage/UniqueUIDGenerator.h"
#include "OTModelEntities/ModelEntitiesAPIExport.h"

// MongoDB header
#include <mongocxx/cursor.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/basic/array.hpp>

// std header
#include <map>
#include <list>
#include <string>

#pragma warning(disable : 4251)

class EntityBase;

class OT_MODELENTITIES_API_EXPORT ModelStateEntity
{
	OT_DECL_DEFCOPY(ModelStateEntity)
	OT_DECL_DEFMOVE(ModelStateEntity)
public:
	enum tEntityType { TOPOLOGY, DATA };

	explicit ModelStateEntity() = default;
	~ModelStateEntity() = default;

	void setParentEntityID(ot::UID id) { m_parentEntityID = id; };
	void setVersion(ot::UID version) { m_entityVersion = version; };
	void setEntityType(tEntityType type) { m_entityType = type; };

	ot::UID getParentEntityID() const { return m_parentEntityID; };
	ot::UID getEntityVersion() const { return m_entityVersion; };
	tEntityType getEntityType() const { return m_entityType; };

	void addToJsonObject(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

private:
	ot::UID m_entityVersion = 0;
	ot::UID m_parentEntityID = 0;
	tEntityType m_entityType = tEntityType::DATA;
};

class OT_MODELENTITIES_API_EXPORT ModelState
{
	OT_DECL_NOCOPY(ModelState)
	OT_DECL_NOMOVE(ModelState)
	friend class FixtureModelState;

public:
	struct VersionInformation
	{
		std::string branch; //! @brief The branch the version belongs to.
		std::string version; //! @brief The version string.
		bool isEndOfBranch = false; //! @brief Whether this version is the end of its branch.
		bool isOriginalBranch = false; //! @brief Whether this version is part of the original branch.
	};

	//! @brief Construcor for a read-only ModelState.
	//! A read-only model state can be modified in memory but the "write to database" methods won't work.
	ModelState();

	//! @brief Constructor for a writeable ModelState class.
	//! A writeable model state can be modified in memory and written to the database.
	//! @param _sessionID The session ID for the model state.
	//! @param _serviceID The service ID for the model state.
	ModelState(unsigned int _sessionID, unsigned int _serviceID);

	~ModelState();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Gerneral

	// Reset the model state to an empty project
	void reset();

	// Check whether the model state has been modified since the last save
	bool isModified() const { return m_stateModified; };

	//! @brief Create and return a new entity ID.
	OT_DECL_NODISCARD ot::UID createEntityUID();

	//! @brief Get the currently active model state from the database.
	//! @param _information A reference to a VersionInformation object that will be filled with the information of the active model state if it exists.
	//! @return An optional containing the BSON document of the active model state if it exists, or std::nullopt if it does not exist.
	OT_DECL_NODISCARD std::optional<bsoncxx::v_noabi::document::value> getActiveModelState(VersionInformation& _information) const;

	//! @brief Open a project, load the version group and the currently active version.
	OT_DECL_NODISCARD bool openProject(const std::string& _customVersion = std::string());

	//! @brief Load a model state with a particular version.
	OT_DECL_NODISCARD bool loadModelState(const std::string& _version, bool _loadFromGraph = true);

	//! @brief Save the current modified model state. The version counter is incremented automatically in the last digit (e.g. 1.2.1 -> 1.2.2)
	OT_DECL_NODISCARD bool saveModelState(bool forceSave, bool forceAbsoluteState, const std::string& saveComment);

	// Load the version graph of the model (needs to be called when the project is opened)
	void loadVersionGraph();

	//! @brief Check the database schema version and upgrade, if needed.
	//! @throws ot::InvalidVersionException if the model state is read-only and the schema version is not compatible with the current application version.
	void checkAndUpgradeDataBaseSchema();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Entity handling

	// Store an entity to the data base (it will be automatically determine whetehr the entity is new or modified)
	void storeEntity(ot::UID entityID, ot::UID parentEntityID, ot::UID entityVersion, ModelStateEntity::tEntityType entityType);

	// Add new entity to model state
	void addNewEntity(ot::UID entityID, ot::UID parentEntityID, ot::UID entityVersion, ModelStateEntity::tEntityType entityType);

	// Mark entity as modified (new version and parent)
	void modifyEntity(ot::UID entityID, ot::UID parentEntityID, ot::UID entityVersion, ModelStateEntity::tEntityType entityType);

	// Mark entity as modified (new version)
	void modifyEntityVersion(const EntityBase& _entity);

	// Mark entity as modified (new version)
	void modifyEntityVersion(ot::UID entityID, ot::UID entityVersion);

	// Mark entity as modified (new parent)
	void modifyEntityParent(ot::UID entityID, ot::UID parentEntityID);

	// Remove entity from model state
	void removeEntity(ot::UID entityID, bool considerChildren = true);

	// Deactivate the latest model state and reload an earlier state. Returns true if the state could be reverted (needs to have at least one more model state)
	bool undoLastOperation();

	// Re-activate the next inactive state and reload the model state
	bool redoNextOperation();

	// Delete all model entities which were written after the last model state and therefore are not referred to in the data base
	void removeDanglingModelEntities();

	// Remove all redo model states and the items belonging to them 
	OT_DECL_NODISCARD std::list<std::string> removeRedoModelStates();

	// Write the version information to the entity in the data base
	void updateVersionEntity(const std::string& _version);

	//! @brief Restore the orginal version if needed.
	//! Will only modify the model entity without loading the model state.
	//! @note This function should be called immediately before closing the project.
	void restoreOriginalVersionIfNeeded();

	// Build the information map containing the list of children for each item
	void buildChildrenInformation();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Project preview image and description

	//! @brief Reads the current project preview image of the project.
	//! @param _collectionName The name of the project collection to read the preview image from.
	//! @param _imageData The image data read.
	//! @param _format The image format read.
	//! @return True on success, false if the project has no preview image or an error occurred.
	OT_DECL_NODISCARD static bool readProjectPreviewImage(const std::string& _collectionName, std::vector<char>& _imageData, ot::ImageFileFormat _format);

	//! @brief Reads the current project description.
	//! @param _collectionName The name of the project collection to read the description from.
	//! @param _description The project description read.
	//! @return True on success, false if no description is found or an error occurred.
	OT_DECL_NODISCARD static bool readProjectDescription(const std::string& _collectionName, std::string& _description, ot::DocumentSyntax& _syntax);

	//! @brief Adds an image to the project.
	//! Any existing image will be replaced.
	//! @param _imageData The image data to add.
	bool addPreviewImage(std::vector<char>&& _imageData, ot::ImageFileFormat _format);

	//! @brief Removes the current project preview image if one exists.
	void removePreviewImage();

	//! @brief Adds or updates the project description.
	//! @param _description The project description to add.
	bool addProjectDescription(const std::string& _description, ot::DocumentSyntax _syntax);

	//! @brief Removes the current project description if one exists.
	void removeProjectDescription();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Getter

	// Determine the current modelStateVersion (the last saved one)
	const std::string& getModelStateVersion() const { return m_graphCfg.getActiveVersionName(); };

	// Determine the currently active branch
	const std::string& getActiveBranch() const { return m_graphCfg.getActiveBranchName(); }

	// Determine the current version of an entity
	ot::UID getCurrentEntityVersion(ot::UID entityID) const;

	// Determine the parent of an entity
	ot::UID getCurrentEntityParent(ot::UID entityID) const;

	//! @brief Fills the provided list with all topology entity IDs in the model.
	void getListOfTopologyEntities(ot::UIDList& _topologyEntityIDs) const;

	//! @brief Fills the provided list with all topology entities in the model.
	void getListOfTopologyEntities(std::list<std::pair<ot::UID, ModelStateEntity>>& _topologyEntities) const;

	//! @broef Determine whether a given version is part of the currently active branch.
	bool isVersionInActiveBranch(const std::string& version) const;

	//! \brief Returns true if the specified version has at least one child.
	bool hasNextVersion(const std::string& _version) const;

	// Get the next version in the active branch following the given one
	std::string getNextVersion(const std::string& version) const;

	// Get the previous version in the active branch before the given one
	std::string getPreviousVersion(const std::string& version) const;

	std::string getPrecedingVersion(const std::string& _version) const;

	std::string getProceedingVersion(const std::string& _currentVersion, const std::string& _targetVersion) const;

	// Get the description for the given version
	std::string getVersionDescription(const std::string& version) const;

	// Check whether an undo operation is possible
	bool canUndo() const;

	// Check whether a redo operation is possible
	bool canRedo() const;

	// Get the description which was stored for the current model state
	std::string getCurrentModelStateDescription() const;

	// Get the description which was stored for the next redo model state
	std::string getRedoModelStateDescription() const;

	// Get a list of all model states (version, description);
	ot::VersionGraphCfg& getVersionGraph() { return m_graphCfg; };

	// Get a list of all model states (version, description);
	const ot::VersionGraphCfg& getVersionGraph() const { return m_graphCfg; };
	
	void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper: Clear

	//! @brief Clars the entire model state information.
	void clearModelState();

	//! @brief Clear the childred relation map.
	void clearChildrenInformation();

	// Remove an entity from the child list of its former parent entity
	void removeEntityFromParent(ot::UID entityID, ot::UID parentID);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper: Load

	// Add an entity to the child list of its parent entity
	void addEntityToParent(ot::UID entityID, ot::UID parentID);

	// This function loads the model state from a given ModelState document
	bool loadModelFromDocument(bsoncxx::document::view docView, bool _loadFromGraph);
	
	// This function loads an absolute model state from a given ModelState document
	bool loadAbsoluteState(bsoncxx::document::view docView);

	// This function loads an incremental (relative) model state from a given ModelState document
	bool loadIncrementalState(bsoncxx::document::view docView);

	// This function loads a state (as modification of the current state) of the expected type
	bool loadState(bsoncxx::document::view docView, const std::string &expectedType);

	// Load the entity data from a state document
	void loadStateData(bsoncxx::document::view docView);

	//! @brief Reads additional project information from the given document view.
	//! Additional project information includes:
	//! 
	//! - Preview image UID/Version/Format
	//! 
	//! - Description UID/Version/Syntax
	//! 
	//! Will clear the UID/version information of the additional information entries if no information is found in the document.
	//! @param _documentView The document view to read the information from.
	//! @return True on success, false otherwise.
	bool readAdditionalProjectInformation(const bsoncxx::v_noabi::document::view& _documentView);

	//! @brief Retrieve all dangling entities from the given cursor and add them to the given array.
	//! Dangling entities are entities which are not part of any model state.
	//! @param _cursor The cursor to retrieve the entities from.
	//! @param _entityArray The array to add the entities to. 
	//! @return 
	bool getDanglingEntities(mongocxx::cursor& _cursor, bsoncxx::builder::basic::array& _entityArray);

	//! @brief Helper to perform schema upgrade from version 1 to version 2.
	//! @throws ot::InvalidVersionException if the model state is read-only and the schema version is not compatible with the current application version.
	void updateSchema_1_2();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper: Save

	// Save the current state as absolute model state
	bool saveAbsoluteState(const std::string &saveComment);

	// Save the current state as absolute model state with extensions
	bool saveAbsoluteStateWithExtension(const std::string &saveComment);

	// Save the current state as relative model state
	bool saveIncrementalState(const std::string &saveComment);

	// Write the main document of an absolute state with extensions
	bool writeMainDocument(std::map<ot::UID, ModelStateEntity> &entitiesLeft, const std::string &saveComment);

	// Write an extension document of an absolute state
	bool writeExtensionDocument(std::map<ot::UID, ModelStateEntity> &entitiesLeft);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	// Helper function to determine the version of the last model entity in the data base
	long long getCurrentModelEntityVersion() const;

	//! @brief Count the number of dots in a version string.
	static int countNumberOfDots(const std::string& _text);

	//! @brief Determine whether a given version is part of the given branch.
	bool isVersionInBranch(const std::string& _versionName, const std::string& _branchName) const;

	//! @brief Get the parent branch of the given branch (remove the part after the last .).
	static std::string getParentBranch(const std::string& _branchName);

	// Activate the branch which contains the given version
	void activateBranch(const std::string& _version);

	// Remove an item from the version graph
	void removeVersionGraphItem(const std::string &version);

	// Determine the last version in the currently active branch
	std::string getLastVersionInActiveBranch() const;

	// Create a new branch and activate it (also update the model entity)
	void createAndActivateNewBranch();

	//! @brief Write the current branch and version to the model entity if data differs from the data in the model entity.
	//! @note If the model state is read-only this function does nothing.
	void storeCurrentVersionInModelEntity();

	//! @brief Attempt to write the given branch and version to the model entity if the version and branch exist in the graph.
	//! @note If the model state is read-only this function does nothing.
	//! @param _branch Branch name to store.
	//! @param _version Version name to store.
	void storeVersionInModelEntityIfExists(const std::string& _branch, const std::string& _version);

	//! @brief Write the given branch and version to the model entity if data differs from the data in the model entity.
	//! @note If the model state is read-only this function does nothing.
	//! @param _branch Branch name to store.
	//! @param _version Version name to store.
	void storeVersionInModelEntity(const std::string& _branch, const std::string& _version);

	// Determine all versions which are following the given version (regardless of the branch)
	void getAllChildVersions(const std::string& _version, std::list<std::string>& _childVersions);
	void getAllChildVersions(const ot::VersionGraphVersionCfg* _version, std::list<std::string>& _childVersions);

	// Delete the given model version (model state) together with all its newly created entities
	void deleteModelVersion(const std::string& version);

	// When we load a relative state, the attribute will hold the version of the last absolute state (base state)
	std::string m_currentModelBaseStateVersion;

	// Information regarding the entities which are currently part of the model
	std::map<ot::UID, ModelStateEntity> m_entities;

	// Information about the added / modified / removed entities with regard to the last saved model state
	std::map<ot::UID, ModelStateEntity> m_addedOrModifiedEntities;
	std::map<ot::UID, ModelStateEntity> m_removedEntities;

	// Information about entity children
	std::map<ot::UID, std::list<ot::UID>> m_entityChildrenList;

	//! @brief If set the model state is read-only and no modifications in the data base are allowed.
	bool m_readOnly;

	// A flag which indicates whether the model state has been modified compared to the last stored state.
	bool m_stateModified;

	// The maximum number of array entities per state
	const size_t m_maxNumberArrayEntitiesPerState;

	// The current number of relative model states after the previous absolute state
	size_t m_relativeModelStateCount;

	// The maximum allowed number of relative model states before an absolute state is written (for undo/redo and model open performance)
	const size_t m_maximualRelativeModelStateCount;
		
	// The active branch which is currently stored in the model entity
	std::string m_activeBranchInModelEntity;

	// The active version which is currently stored in the model entity
	std::string m_activeVersionInModelEntity;

	//! @brief The preview image UID.
	ot::UID m_previewImageUID;

	//! @brief The preview image version.
	ot::UID m_previewImageVersion;

	//! @brief The preview image format.
	ot::ImageFileFormat m_previewImageFormat;

	//! @brief The description entity UID.
	ot::UID m_descriptionUID;

	//! @brief The description entity version.
	ot::UID m_descriptionVersion;

	//! @brief The syntax of the project description.
	ot::DocumentSyntax m_descriptionSyntax;

	// The member for creation of Unique IDs
	ot::UniqueUIDGenerator* m_uniqueUIDGenerator;

	// The version graph (for each version: version, parentVersion, description).
	// The version graph needs to be loaded when the project is opened and will then be kept up to date
	ot::VersionGraphCfg m_graphCfg;

	//! @brief The originally active version when the project was opened (as stored in data base) before applying a custom version if set.
	VersionInformation m_originalInitialVersion;

	//! @brief The custom version that was loaded when the project was opened.
	//! This version is only set if a custom version was specified when opening the project.
	VersionInformation m_customInitialVersion;
};
