// @otlicense
// File: Model.h
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

#include <string>
#include <map>
#include <list>
#include <atomic>

#include "EntityContainer.h"
#include "Geometry.h"
#include "OldTreeIcon.h"
#include "ModelState.h"
#include "EntityBase.h"
#include "EntityFaceAnnotation.h"
#include "OTCore/FolderNames.h"

#include "OTGui/GuiTypes.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/MessageDialogCfg.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "NewModelStateInfo.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/UiComponent.h"

class EntityMesh;
class EntityMeshTet;
class EntityMeshCartesian;
class EntityGeometry;
class EntityBrep;
class EntityMaterial;
class EntityParameter;
class EntityAnnotation;
class TopoDS_Shape;
class MicroserviceNotifier;
class ModalCommandBase;

//! The Model class is the main interface to the model.
//! There may be multiple instances of this class. Each model stores all objects within the model and provides access to the model
//! Entities. Each model Entity is identified by a UID which is automatically created and managed by the model class.
//! 0 is not a valid UID and is returned in case of an error.
class Model : public EntityObserver
{
public:
	// Overrides from EntityObserver
	virtual void entityRemoved(EntityBase *entity) override;
	virtual void entityModified(EntityBase *entity) override;
	virtual void sendMessageToViewer(ot::JsonDocument &doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds) override;
	virtual void requestConfigForModelDialog(const ot::UID& _entityID, const std::string _collectionType, const std::string& _targetFolder, const std::string& _elementType) override;
	virtual void requestVisualisation(ot::UID _entityID, ot::VisualisationCfg& _visualisationCfg) override;

	Model(const std::string &_projectName, const std::string& _projectType, const std::string &_collectionName);
	virtual ~Model();

	ot::UID				createEntityUID();
	void			addEntityToMap(EntityBase *entity);
	void			removeEntityFromMap(EntityBase *entity, bool keepInProject, bool keepParameterDependency, bool considerChildren = true);
	void		    removeEntityWithChildrenFromMap(EntityBase *entity, bool keepInProject, bool keepParameterDependency, std::list<EntityBase*> &removedEntities);

	bool			entityExists(ot::UID uID);
	void			getModelBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax);
	void			setVisualizationModel(ot::UID visModelID);
	ot::UID				getVisualizationModel();
	void			modelItemRenamed(ot::UID entityID, const std::string &newName);
	void			keySequenceActivated(const std::string &keySequence);
	void			updateCurvesInPlot(const std::list<std::string>& curveNames, const ot::UID& plotID);
	std::map<std::string, bool> getListOfEntityNames();
	std::map<std::string, ot::UID> getEntityNameToIDMap();

	void addCommonPropertiesToConfig(const std::list<ot::UID> &entityIDList, bool visibleOnly, ot::PropertyGridCfg& _config);
	void        setPropertiesFromJson(const std::list<ot::UID>& entityIDList, const ot::PropertyGridCfg& _configuration, bool updateEntities, bool itemsVisible);
	void setVersionPropertiesFromJson(const ot::PropertyGridCfg& _configuration);
	void        deleteProperty(const std::list<ot::UID>& entityIDList, const std::string &propertyName, const std::string& propertyGroup);

	bool entitiesNeedUpdate();
	void updateEntities(bool itemsVisible);

	void deleteEntity(EntityBase *entity);
	void facetEntity(EntityGeometry *entity, double deflection, bool isHidden, bool notifyViewer);
	double calculateDeflectionFromListOfEntities(std::list<EntityBase *> &entities);
	double calculateDeflectionFromAllEntities();
	void refreshAllViews();
	void resetAllViews();
	void clearSelection();
	void getAllGeometryEntities(std::list<EntityGeometry *> &geometryEntities);
	void getAllEntities(std::list<EntityBase *> &entities);
	void addEntityToModel(std::string entityPath, EntityBase *entity, EntityBase *root, bool addVisualizationContainers, std::list<EntityBase *> &newEntities);
	EntityBase *getRootNode() { return entityRoot; };

	std::string getGeometryRootName() { return "Geometry"; };
	std::string getCircuitsRootName() { return "Circuits"; };
	std::string getMaterialRootName() { return "Materials"; };
	std::string getParameterRootName() { return "Parameters"; };
	std::string getMeshRootName() { return "Meshes"; };
	std::string getSolverRootName() { return ot::FolderNames::SolverFolder; };
	std::string getScriptsRootName() { return ot::FolderNames::PythonScriptFolder; };
	std::string getUnitRootName() { return "Units"; };

	void addVisualizationNodeFromFacetData(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], ot::UID modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
										   double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	void addVisualizationNodeFromFacetDataBase(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool textureReflective, ot::UID modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
										       double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ot::UID entityID, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);

	void addAnnotationEntities(std::list<EntityAnnotation *> &errorAnnotations);
	void addVisualizationAnnotationNode(const std::string &name, ot::UID UID, const OldTreeIcon &treeIcons, bool isHidden,
									    const double edgeColorRGB[3],
										const std::vector<std::array<double, 3>> &points,
										const std::vector<std::array<double, 3>> &points_rgb,
										const std::vector<std::array<double, 3>> &triangle_p1,
									    const std::vector<std::array<double, 3>> &triangle_p2,
									    const std::vector<std::array<double, 3>> &triangle_p3,
									    const std::vector<std::array<double, 3>> &triangle_rgb);

	void updateObjectFacetsFromDataBase(ot::UID modelEntityID, ot::UID entityID);

	void removeShapesFromVisualization(std::list<ot::UID> &removeFromDisplay);
	void setShapeVisibility(std::list<ot::UID> &visibleEntityIDs, std::list<ot::UID> &hiddenEntityIDs);

	void displayMessage(const std::string &message);
	void reportError(const std::string &message);
	void reportWarning(const std::string &message);
	void reportInformation(const std::string &message);

	std::string getProjectType() { return projectType; }

	void entitiesSelected(const std::string &selectionAction, const std::string &selectionInfo, std::map<std::string, std::string> &options);

	void         setModelStorageVersion(ot::UID version) { modelStorageVersion = version; };
	ot::UID getModelStorageVersion() { return modelStorageVersion; };

	void setModified();
	void resetModified();
	bool getModified() { return isModified; };
	
	void resetToNew();

	bool GetDocumentFromEntityID(ot::UID entityID, bsoncxx::builder::basic::document &doc);

	//! @brief Restores entity from database. Topology entities are added to the entity map. 
	//! The map is íntended to be the centralized map in the model service, which is only supposed to hold topology entities.
	EntityBase *readEntityFromEntityID(EntityBase *parent, ot::UID entityID, std::map<ot::UID, EntityBase *> &entityMap);
	EntityBase *readEntityFromEntityIDandVersion(EntityBase *parent, ot::UID entityID, ot::UID version, std::map<ot::UID, EntityBase *> &entityMap);

	void prefetchDocumentsFromStorage(std::list<ot::UID> &prefetchIds);

	void addVisualizationContainerNode(const std::string &name, ot::UID entityID, bool isEditable);
	void addVisualizationMeshNode(const std::string &name, ot::UID entityID);

	bool isProjectOpen() const { return m_isProjectOpen; };
	void projectOpen(const std::string& _customVersion = std::string());
	void projectSave(const std::string &comment, bool silentlyCreateBranch);

	void detachAllViewer();

	void updatePropertyGrid();

	void enableQueuingHttpRequests(bool flag);

	void modelChangeOperationCompleted(const std::string &description, bool askForCreationOfBranch = true);

	void uiIsAvailable(ot::components::UiComponent* _ui);

	std::string getCurrentModelVersion();
	void activateVersion(const std::string &version);
	void versionSelected(const std::string &version);
	void versionDeselected();
	void setVersionLabel(const std::string& version, const std::string& label);

	void addEntitiesToModel(const ot::NewModelStateInfo& _modelStateInfo, const std::string& _description, bool _saveModel, bool _askForCreationOfBranch, bool _considerVisualization);
	void addEntitiesToModel(const std::list<ot::UID>& _topologyEntityIDList, const std::list<ot::UID>& _topologyEntityVersionList, const std::list<bool>& _topologyEntityForceVisible, const std::list<ot::UID>& _dataEntityIDList, const std::list<ot::UID>& _dataEntityVersionList, const std::list<ot::UID>& _dataEntityParentList, const std::string& _description, bool _saveModel, bool _askForCreationOfBranch, bool _considerVisualization);
	void addGeometryOperation(ot::UID geomEntityID, ot::UID geomEntityVersion, const std::string &geomEntityName, std::list<ot::UID> &dataEntityIDList, std::list<ot::UID> &dataEntityVersionList, std::list<ot::UID> &dataEntityParentList, std::list<std::string> &childrenList, const std::string &description);
	void deleteEntitiesFromModel(const ot::UIDList& _entityIDList, bool _saveModel);
	void deleteEntitiesFromModel(const std::list<std::string>& _entityNameList, bool _saveModel);
	void deleteEntitiesFromModel(const std::list<EntityBase*>& _entityList, bool _saveModel);
	void updateVisualizationEntity(ot::UID visEntityID, ot::UID visEntityVersion, ot::UID binaryDataItemID, ot::UID binaryDataItemVersion);
	void updateGeometryEntity(ot::UID geomEntityID, ot::UID brepEntityID, ot::UID brepEntityVersion, ot::UID facetsEntityID, ot::UID facetsEntityVersion, bool overrideGeometry, const ot::PropertyGridCfg& _configuration, bool updateProperties);
	
	void updateTopologyEntities(const ot::NewModelStateInfo& _modelStateInfo, const std::string& _comment, bool _considerVisualization);

	//! @brief Performs an update on a topology entity. The updated entity replaces its old version in the entity map and redirects all parent/child relationships to the updated entity.,
	//! @param topoEntityID 
	//! @param topoEntityVersion 
	//! @param comment 
	void updateTopologyEntities(const ot::UIDList& _topoEntityID, const ot::UIDList& _topoEntityVersion, const std::string& _comment, bool _considerVisualization);
	void requestUpdateVisualizationEntity(ot::UID visEntityID);
	std::list<ot::UID> getNewEntityIDs(unsigned long long count);
	std::list<std::string> getListOfFolderItems(const std::string &folder, bool recursive);
	std::list<ot::UID> getIDsOfFolderItemsOfType(const std::string &folder, const std::string &className, bool recursive);
	std::list<EntityBase*> getListOfSelectedEntities(const std::string &typeFilter = "");
	void addPropertiesToEntities(std::list<ot::UID> &entityIDList, const ot::PropertyGridCfg& _configuration);
	void updatePropertiesOfEntities(std::list<ot::UID> &entityIDList, const std::string& _entitiesJson);
	void getListOfAllChildEntities(EntityBase* entity, std::list<std::pair<ot::UID, ot::UID>>& childrenEntities);

	void getEntityVersions(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersions);
	void getEntityNames(std::list<ot::UID> &entityIDList, std::list<std::string> &entityNames);
	void getEntityTypes(std::list<ot::UID> &entityIDList, std::list<std::string> &entityTypes);
	std::list<ot::UID> getAllGeometryEntitiesForMeshing();
	void getEntityProperties(ot::UID entityID, bool recursive, const std::string &propertyGroupFilter, std::map<ot::UID, ot::PropertyGridCfg>& _entityProperties);

	EntityBase *findEntityFromName(const std::string &name);

	void addModalCommand(ModalCommandBase *command);
	void removeModalCommand(ModalCommandBase *command);

	void hideEntities(std::list<ot::UID> &hiddenEntityIDs);

	void requestImportTableFile(const std::string& _itemName);

	void setMeshingActive(ot::UID meshEntityID, bool flag);
	bool getMeshingActive(ot::UID meshEntityID);

	std::string checkParentUpdates(std::list<ot::UID> &modifiedEntities);

	int getServiceID();
	int getSessionCount();

	ModelState* getStateManager();
	void setStateMangager(ModelState* state);

	void promptResponse(const std::string& _type, ot::MessageDialogCfg::BasicButton _answer, const std::string& _parameter1);

	EntityBase* getEntityByID(ot::UID _entityID) const;

	std::map<ot::UID, EntityBase*>& getAllEntitiesByUID() { return entityMap; }
private:
	// Methods
	void clearAll();
	void setupUIControls(ot::components::UiComponent* _ui);
	void createVisualizationItems();
	
	EntityParameter* createNewParameterItem(const std::string &parameterName);
	void addEntityNamesToList(EntityBase *entity, std::map<std::string, bool> &list);
	void addEntityIDsToList(EntityBase *entity, std::map<std::string, ot::UID> &list);
	void addEntityUIToList(EntityBase *entity, std::map<ot::UID, bool> &list);
	std::list<EntityBase *> removeChildrenFromList(std::list<EntityBase *> selectedEntities);
	bool isProtectedEntity(EntityBase *entity);
	void setEntityOutdated(EntityBase *entity);
	void setEntityUpdated(EntityBase *entity);
	void clearEntityUpdates(EntityBase *entity);
	void updateEntity(EntityBase *entity);
	void getCommonProperties(const std::list<EntityBase *> &entities, EntityProperties &props);
	void setProperties(const std::list<EntityBase *> &entities, EntityProperties &props);
	void addGeometryEntitiesToList(EntityBase *root, std::list<EntityGeometry *> &geometryEntities);
	void addAllEntitiesToList(EntityBase *root, std::list<EntityBase *> &allEntities);
	bool anyMeshItemSelected(std::list<ot::UID> &selectedEntityID);
	bool anyMaterialItemSelected(std::list<ot::UID> &selectedEntityID);
	void getFaceCurvatureRadius(const TopoDS_Shape *shape, std::list<double> &faceCurvatureRadius);
	void createFaceAnnotation(const std::list<EntityFaceAnnotationData> &annotations, double r, double g, double b, const std::string &baseName);
	void updateAnnotationGeometry(EntityFaceAnnotation *annotationEntity);
	std::string findMostRecentModelInStorage();
	void performSpecialUpdates(EntityBase *entity);
	void performEntityMeshUpdate(EntityMeshTet *entity);
	void performEntityMeshUpdate(EntityMeshCartesian *entity);
	void removeAllNonTemplateEntities();
	void updateUndoRedoStatus();
	void updateModelStateForUndoRedo();
public:
	void importTableFile(const std::string &fileName, bool removeFile);
private:
	void loadDefaultMaterials();
	void findFacesAtIndexFromShape(EntityFaceAnnotation *annotationEntity, std::list<TopoDS_Shape> &facesList, int faceIndex, EntityBrep* brep);
	void recursiveReplaceEntityName(EntityBase *entity, const std::string &oldName, const std::string &newName, std::list<EntityBase*> &entityList);
	void addChildrenEntitiesToList(EntityGeometry* entity, std::list<std::pair<ot::UID, ot::UID>>& childrenEntities);
	void recursivelyAddEntityInfo(EntityBase *entity, std::map<ot::UID, ot::UID> &entityVersionMap);
	void determineIDandVersionForEntityWithChildren(EntityBase *entity, std::list<ot::UID> &entityInfoIDList, std::list<ot::UID> &entityInfoVersionList);
	void otherServicesUpdate(std::map<std::string, std::list<std::pair<ot::UID, ot::UID>>> otherServicesUpdate, bool itemsVisible);
	void performUpdateVisualizationEntity(std::list<ot::UID> entityIDs, std::list<ot::UID> entityVersions, std::list<ot::UID> brepVersions, std::string owningService);
	bool updateNumericalValues(EntityBase *entity);
	bool evaluateExpressionDouble(const std::string &expression, double &value, ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup);
	void setParameter(const std::string &name, double value, EntityParameter *parameter);
	void removeParameter(const std::string &name);
	void setParameterDependency(std::list<std::string> &parameters, ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup);
	void removeParameterDependency(ot::UID entityID);
	bool checkCircularParameterDependency(EntityParameter *parameter, std::list<std::string> &dependingOnParameters);
	bool checkWhetherParameterDependsOnGivenParameter(EntityParameter *parameter, ot::UID dependingEntityID);
	void replaceParameterByString(EntityParameter *parameter, const std::string &value);
	std::string replaceParameterInExpression(std::string expression, const std::string &parameterName, const std::string &newString);
	bool checkParameterName(const std::string &parameterName);
	bool isValidParameterNameCharacter(char c);
	void sendVersionGraphToUI(const ot::VersionGraphCfg& _versionGraph, const std::string& _currentVersion, std::string _activeBranch);
	void updateVersionGraph();
	void setActiveVersionTreeState();
	void removeVersionGraphVersions(const std::list<std::string> &versions);
	void addNewVersionTreeStateAndActivate(const std::string& _branch, const ot::VersionGraphVersionCfg& _version);
	void getIDsOfFolderItemsOfType(EntityContainer *container, const std::string &className, bool recursive, std::list<ot::UID> &itemList);
	size_t getNumberOfVisualizationTriangles(std::list<EntityGeometry *> geometryEntities);
	std::list<EntityBase*> getListOfEntitiesToConsiderForPropertyChange(const std::list<EntityBase*>& entities);
	void getEntityProperties(EntityBase* entity, bool recursive, const std::string& propertyGroupFilter, std::map<ot::UID, ot::PropertyGridCfg>& _entityProperties);
	void addTopologyEntitiesToModel(std::list<EntityBase*> entities, const std::list<bool>& forceVisible, bool _considerVisualization);
	void removeParentsOfProtected(std::list<EntityBase*>& unprotectedEntities, const std::list<EntityBase*>& protectedEntities);
	std::list<EntityBase*> FindTopLevelBlockEntities(std::list<EntityBase*>& entityID);
	std::list<EntityBase*> getTopLevelEntitiesByName(std::list<EntityBase*> entities);

	// Button callbacks
	ot::ButtonHandler m_buttonHandler;
	ot::ToolBarButtonCfg m_undoButton;
	ot::ToolBarButtonCfg m_redoButton;
	ot::ToolBarButtonCfg m_deleteButton;
	ot::ToolBarButtonCfg m_infoButton;
	ot::ToolBarButtonCfg m_saveButton;
	ot::ToolBarButtonCfg m_createParameterButton;

	void handleUndoLastOperation();
	void handleRedoNextOperation();
	void handleDeleteSelectedShapes();
	void handleShowSelectedShapeInformation();
	void handleCreateNewParameter();

	// Persistent attributes (need to be stored in data base)
	EntityContainer               *entityRoot;

	ot::UID modelStorageVersion;
	bool anyDataChangeSinceLastWrite;

	// Temporary attributes (need to be rebuilt when the model is loaded)
	std::map<ot::UID, EntityBase *>    entityMap;
	std::map<EntityBase*, bool>	   pendingEntityUpdates;
	
	// Temporary attributes
	bool                           m_isProjectOpen;
	ot::UID							visualizationModelID;
	bool						   isModified;
	std::string					   projectName;
	std::string					   projectType;
	std::string					   collectionName;
	
	bool						   uiCreated;
	bool						   versionGraphCreated;
	std::string					   newTableItemName;
	std::map<ot::UID, bool>		   meshingActive;
	std::string                    m_selectedVersion;

	std::map<std::string, std::pair<double, EntityParameter*>>  parameterMap;

	Model() = delete;
	Model(const Model&) = delete;

	// Model state manager
	ModelState *stateManager;

	// Flag indicating whether the model is currently shutting down (no more modified messages are sent)
	bool shutdown;

	// List of Modal operations
	std::list<ModalCommandBase *> modalCommands;
};



