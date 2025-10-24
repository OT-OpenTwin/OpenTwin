#pragma once
#pragma warning(disable : 4251)

#include <map>
#include <string>
#include <list>
#include "UniqueUIDGenerator.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/VersionGraphCfg.h"

#include <mongocxx/cursor.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/basic/array.hpp>


class EntityBase;

class __declspec(dllexport) ModelStateEntity
{
public:

	enum tEntityType { TOPOLOGY, DATA };

	ModelStateEntity() : m_entityVersion(0), m_parentEntityID(0), m_entityType(tEntityType::DATA) {};
	~ModelStateEntity() {};

	void setParentEntityID(ot::UID id) { m_parentEntityID = id; };
	void setVersion(ot::UID version) { m_entityVersion = version; };
	void setEntityType(tEntityType type) { m_entityType = type; };

	ot::UID getParentEntityID(void) const { return m_parentEntityID; };
	ot::UID getEntityVersion(void) const { return m_entityVersion; };
	tEntityType getEntityType(void) const { return m_entityType; };

private:
	ot::UID m_entityVersion;
	ot::UID m_parentEntityID;
	tEntityType m_entityType;
};

class __declspec(dllexport) ModelState
{
public:
	ModelState() = delete;
	ModelState(unsigned int sessionID, unsigned int serviceID);
	~ModelState();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Gerneral

	// Reset the model state to an empty project
	void reset(void);

	// Check whether the model state has been modified since the last save
	bool isModified(void) const { return m_stateModified; };

	// Open a project, load the version grap and the currently active version
	bool openProject(const std::string& _customVersion = std::string());

	// ###########################################################################################################################################################################################################################################################################################################################

	// Entity handling

	// Create and return a new entity ID (this will increate the maximum entity ID and mark the model state as modified
	unsigned long long createEntityUID(void);

	// Load a model state with a particular version
	bool loadModelState(const std::string& _version);

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

	// Determine the current modelStateVersion (the last saved one)
	const std::string& getModelStateVersion(void) const { return m_graphCfg.getActiveVersionName(); };

	// Determine the currently active branch
	const std::string& getActiveBranch(void) const { return m_graphCfg.getActiveBranchName(); }

	// Save the current modified model state. The version counter is incremented automatically in the last digit (e.g. 1.2.1 -> 1.2.2)
	bool saveModelState(bool forceSave, bool forceAbsoluteState, const std::string &saveComment);

	// Determine the current version of an entity
	ot::UID getCurrentEntityVersion(ot::UID entityID);

	// Determine the parent of an entity
	ot::UID getCurrentEntityParent(ot::UID entityID);

	// Get a list of all topology entities in the model
	void getListOfTopologyEntites(std::list<unsigned long long> &topologyEntities);

	// Deactivate the latest model state and reload an earlier state. Returns true if the state could be reverted (needs to have at least one more model state)
	bool undoLastOperation(void);

	// Re-activate the next inactive state and reload the model state
	bool redoNextOperation(void);

	// Get the description which was stored for the current model state
	std::string getCurrentModelStateDescription(void);

	// Get the description which was stored for the next redo model state
	std::string getRedoModelStateDescription(void);

	// Check whether an undo operation is possible
	bool canUndo(void);

	// Check whether a redo operation is possible
	bool canRedo(void);

	// Delete all model entities which were written after the last model state and therefore are not referred to in the data base
	void removeDanglingModelEntities(void);

	// Load the version graph of the model (needs to be called when the project is opened)
	void loadVersionGraph(void);

	// Get a list of all model states (version, description);
	ot::VersionGraphCfg& getVersionGraph(void);

	// Get a list of all model states (version, description);
	const ot::VersionGraphCfg& getVersionGraph(void) const;

	// Check the database schema version and upgrade, if needed
	void checkAndUpgradeDataBaseSchema(void);

	// Remove all redo model states and the items belonging to them 
	std::list<std::string> removeRedoModelStates(void);
	
	// Write the version information to the entity in the data base
	void updateVersionEntity(const std::string& _version);

	//! @brief Adds an image to the project.
	//! Any existing image will be replaced.
	//! @param _imageData The image data to add.
	bool addPreviewImage(std::vector<char>&& _imageData, ot::ImageFileFormat _format);

	//! @brief Reads the current project preview image of the project.
	//! @param _collectionName The name of the project collection to read the preview image from.
	//! @param _imageData The image data read.
	//! @param _format The image format read.
	//! @return True on success, false if the project has no preview image or an error occurred.
	static bool readProjectPreviewImage(const std::string& _collectionName, std::vector<char>& _imageData, ot::ImageFileFormat _format);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	// This function loads the model state from a given ModelState document
	bool loadModelFromDocument(bsoncxx::document::view docView);
	
	// This function loads an absolute model state from a given ModelState document
	bool loadAbsoluteState(bsoncxx::document::view docView);

	// Clear the information map containing the list of children for each item
	void clearChildrenInformation(void);

	// Build the information map containing the list of children for each item
	void buildChildrenInformation(void);
	
	// Add an entity to the child list of its parent entity
	void addEntityToParent(ot::UID entityID, ot::UID parentID);

	// Remove an entity from the child list of its former parent entity
	void removeEntityFromParent(ot::UID entityID, ot::UID parentID);

	// This function loads an incremental (relative) model state from a given ModelState document
	bool loadIncrementalState(bsoncxx::document::view docView);

	// This function loads a state (as modification of the current state) of the expected type
	bool loadState(bsoncxx::document::view docView, const std::string &expectedType);

	// This function clears the entire model state information
	void clearModelState(void);

	// Load the entity data from a state document
	void loadStateData(bsoncxx::document::view docView);

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

	//! @brief Retrieve all dangling entities from the given cursor and add them to the given array.
	//! Dangling entities are entities which are not part of any model state.
	//! @param _cursor The cursor to retrieve the entities from.
	//! @param _entityArray The array to add the entities to. 
	//! @return 
	bool getDanglingEntities(mongocxx::cursor& _cursor, bsoncxx::builder::basic::array& _entityArray);

	// Determine whether a given version is part of the currently active branch
	bool isVersionInActiveBranch(const std::string &version);

	// Determine whether a given version is part of the given branch
	bool isVersionInBranch(const std::string &version, const std::string &branch);

	//! @brief Reads the project preview information from the given document view.
	//! If no preview information is found, the preview image UID will be set to invalid.
	//! @param _documentView The document view to read the information from.
	//! @return True on success, false otherwise.
	bool readProjectPreviewInformation(bsoncxx::v_noabi::document::view& _documentView);

	// Get the parent branch of the given branch (remove the part after the last .)
	std::string getParentBranch(const std::string &branch);

	// Activate the branch which contains the given version
	void activateBranch(const std::string& _version);

	//! \brief Returns true if the specified version has at least one child.
	bool hasNextVersion(const std::string& _version);

	// Get the next version in the active branch following the given one
	std::string getNextVersion(const std::string &version);

	// Get the previous version in the active branch before the given one
	std::string getPreviousVersion(const std::string &version);

	// Get the description for the given version
	std::string getVersionDescription(const std::string &version);

	// Remove an item from the version graph
	void removeVersionGraphItem(const std::string &version);

	// Helper function to determine the version of the last model entity in the data base
	long long getCurrentModelEntityVersion(void);

	// Determine the last version in the currently active branch
	std::string getLastVersionInActiveBranch(void);
 
	// Write the current active branch and model version to the model entity
	void storeCurrentVersionInModelEntity(void);

	// Delete the given model version (model state) together with all its newly created entities
	void deleteModelVersion(const std::string &version);

	// Determine all versions which are following the given version (regardless of the branch)
	void getAllChildVersions(const std::string& _version, std::list<std::string>& _childVersions);
	void getAllChildVersions(const ot::VersionGraphVersionCfg* _version, std::list<std::string>& _childVersions);

	// Create a new branch and activate it (also update the model entity)
	void createAndActivateNewBranch(void);

	// Count the number of dots in a version string
	int countNumberOfDots(const std::string &text);

	// Helper to perform schema upgrade from version 1 to version 2
	void updateSchema_1_2(void);

	// When we load a relative state, the attribute will hold the version of the last absolute state (base state)
	std::string m_currentModelBaseStateVersion;

	// Information regarding the entities which are currently part of the model
	std::map<ot::UID, ModelStateEntity> m_entities;

	// Information about the added / modified / removed entities with regard to the last saved model state
	std::map<ot::UID, ModelStateEntity> m_addedOrModifiedEntities;
	std::map<ot::UID, ModelStateEntity> m_removedEntities;

	// Information about entity children
	std::map<ot::UID, std::list<ot::UID>> m_entityChildrenList;

	// A flag which indicates whether the model state has been modified compared to the last stored state.
	bool m_stateModified;

	std::string m_initialBranch; //! @brief The initial branch when the project was opened.
	std::string m_initialVersion; //! @brief The initial version when the project was opened.

	//! @brief Indicates whether the custom version to open is the end of a branch.
	bool m_customVersionIsEndOfBranch;

	// The maximum number of array entities per state
	const size_t m_maxNumberArrayEntitiesPerState;
		
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

	// The member for creation of Unique IDs
	DataStorageAPI::UniqueUIDGenerator* m_uniqueUIDGenerator;

	// The version graph (for each version: version, parentVersion, description).
	// The version graph needs to be loaded when the project is opened and will then be kept up to date
	ot::VersionGraphCfg m_graphCfg;
};
