#pragma once
#pragma warning(disable : 4251)

#include <map>
#include <string>
#include <list>
#include "UniqueUIDGenerator.h"
#include "OTGui/VersionGraphCfg.h"

#include <mongocxx/cursor.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/basic/array.hpp>

class __declspec(dllexport) ModelStateEntity
{
public:
	typedef unsigned long long EntityID;
	typedef long long EntityVersion;

	enum tEntityType { TOPOLOGY, DATA };

	ModelStateEntity() : entityVersion(0), parentEntityID(0), entityType(tEntityType::DATA) {};
	~ModelStateEntity() {};

	void setParentEntityID(EntityID id) { parentEntityID = id; };
	void setVersion(EntityVersion version) { entityVersion = version; };
	void setEntityType(tEntityType type) { entityType = type; };

	EntityID getParentEntityID(void) { return parentEntityID; };
	EntityVersion getEntityVersion(void) { return entityVersion; };
	tEntityType getEntityType(void) { return entityType; };

private:
	EntityVersion entityVersion;
	EntityID parentEntityID;
	tEntityType entityType;
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
	bool isModified() { return stateModified; };

	// Open a project, load the version grap and the currently active version
	bool openProject(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Entity handling

	// Create and return a new entity ID (this will increate the maximum entity ID and mark the model state as modified
	unsigned long long createEntityUID(void);

	// Load a model state with a particular version
	bool loadModelState(const std::string& _version);

	// Store an entity to the data base (it will be automatically determine whetehr the entity is new or modified)
	void storeEntity(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentEntityID, ModelStateEntity::EntityVersion entityVersion, ModelStateEntity::tEntityType entityType);

	// Add new entity to model state
	void addNewEntity(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentEntityID, ModelStateEntity::EntityVersion entityVersion, ModelStateEntity::tEntityType entityType);

	// Mark entity as modified (new version and parent)
	void modifyEntity(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentEntityID, ModelStateEntity::EntityVersion entityVersion, ModelStateEntity::tEntityType entityType);

	// Mark entity as modified (new version)
	void modifyEntityVersion(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityVersion entityVersion);

	// Mark entity as modified (new parent)
	void modifyEntityParent(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentEntityID);

	// Remove entity from model state
	void removeEntity(ModelStateEntity::EntityID entityID, bool considerChildren = true);

	// Determine the current modelStateVersion (the last saved one)
	std::string getModelStateVersion(void) { return m_graphCfg.getActiveVersionName(); };

	// Determine the currently active branch
	std::string getActiveBranch(void) { return m_graphCfg.getActiveBranchVersionName(); }

	// Save the current modified model state. The version counter is incremented automatically in the last digit (e.g. 1.2.1 -> 1.2.2)
	bool saveModelState(bool forceSave, bool forceAbsoluteState, const std::string &saveComment);

	// Determine the current version of an entity
	ModelStateEntity::EntityVersion getCurrentEntityVersion(ModelStateEntity::EntityID entityID);

	// Determine the parent of an entity
	ModelStateEntity::EntityID getCurrentEntityParent(ModelStateEntity::EntityID entityID);

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
	void addEntityToParent(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentID);

	// Remove an entity from the child list of its former parent entity
	void removeEntityFromParent(ModelStateEntity::EntityID entityID, ModelStateEntity::EntityID parentID);

	// This function loads an incremental (relative) model state from a given ModelState document
	bool loadIncrementalState(bsoncxx::document::view docView);

	// This function loads a state (as modification of the current state) of the expected type
	bool loadState(bsoncxx::document::view docView, const std::string &expectedType);

	// This function clears the entire model state information
	void clearModelState(void);

	// Increment the version of the current model state
	void incrementVersion(void);

	// Load the entity data from a state document
	void loadStateData(bsoncxx::document::view docView);

	// Save the current state as absolute model state
	bool saveAbsoluteState(const std::string &saveComment);

	// Save the current state as absolute model state with extensions
	bool saveAbsoluteStateWithExtension(const std::string &saveComment);

	// Save the current state as relative model state
	bool saveIncrementalState(const std::string &saveComment);

	// Write the main document of an absolute state with extensions
	bool writeMainDocument(std::map<ModelStateEntity::EntityID, ModelStateEntity> &entitiesLeft, const std::string &saveComment);

	// Write an extension document of an absolute state
	bool writeExtensionDocument(std::map<ModelStateEntity::EntityID, ModelStateEntity> &entitiesLeft);

	// Helper function to determine non-modelstate entries in a list of results
	bool getListOfNonModelStateEntities(mongocxx::cursor &cursor, bsoncxx::builder::basic::array &entityArray);

	// Determine whether a given version is part of the currently active branch
	bool isVersionInActiveBranch(const std::string &version);

	// Determine whether a given version is part of the given branch
	bool isVersionInBranch(const std::string &version, const std::string &branch);

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

	// Add an item to the version graph
	void addVersionGraphItem(const std::string& _version, const std::string& _parentVersion, const std::string& _label, const std::string& _description);

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

	// Check whether the specified branch already exists
	bool branchExists(const std::string& _branch);

	// Count the number of dots in a version string
	int countNumberOfDots(const std::string &text);

	// Helper to perform schema upgrade from version 1 to version 2
	void updateSchema_1_2(void);

	// When we load a relative state, the attribute will hold the version of the last absolute state (base state)
	std::string currentModelBaseStateVersion;

	// Information regarding the entities which are currently part of the model
	std::map<ModelStateEntity::EntityID, ModelStateEntity> entities;

	// Information about the added / modified / removed entities with regard to the last saved model state
	std::map<ModelStateEntity::EntityID, ModelStateEntity> addedOrModifiedEntities;
	std::map<ModelStateEntity::EntityID, ModelStateEntity> removedEntities;

	// Information about entity children
	std::map<ModelStateEntity::EntityID, std::list<ModelStateEntity::EntityID>> entityChildrenList;

	// A flag which indicates whether the model state has been modified compared to the last stored state.
	bool stateModified;

	// The maximum number of array entities per state
	const size_t maxNumberArrayEntitiesPerState;
		
	// The active branch which is currently stored in the model entity
	std::string activeBranchInModelEntity;

	// The active version which is currently stored in the model entity
	std::string activeVersionInModelEntity;

	// The member for creation of Unique IDs
	DataStorageAPI::UniqueUIDGenerator *uniqueUIDGenerator;

	// The version graph (for each version: version, parentVersion, description).
	// The version graph needs to be loaded when the project is opened and will then be kept up to date
	ot::VersionGraphCfg m_graphCfg;
};
