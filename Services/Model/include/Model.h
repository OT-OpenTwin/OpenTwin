#pragma once

#include <string>
#include <map>
#include <list>

#include "EntityContainer.h"
#include "Geometry.h"
#include "Types.h"
#include "ModelState.h"
#include "EntityBase.h"
#include "ClassFactoryModel.h"
#include "EntityFaceAnnotation.h"

#include "OTCommunication/UiTypes.h"

class EntityMesh;
class EntityMeshTet;
class EntityMeshCartesian;
class EntityGeometry;
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

	Model(const std::string &_projectName, const std::string& _projectType, const std::string &_collectionName);
	virtual ~Model();

	ot::UID				createEntityUID(void);
	void			addEntityToMap(EntityBase *entity);
	void			removeEntityFromMap(EntityBase *entity, bool keepInProject, bool keepParameterDependency, bool considerChildren = true);
	void		    removeEntityWithChildrenFromMap(EntityBase *entity, bool keepInProject, bool keepParameterDependency, std::list<EntityBase*> &removedEntities);


	EntityBase *	getEntity(ot::UID uID);
	bool			entityExists(ot::UID uID);
	void			getModelBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax);
	void			setVisualizationModel(ot::UID visModelID);
	ot::UID				getVisualizationModel(void);
	void			executeAction(const std::string &action, ot::JsonDocument &doc);
	void			executeFunction(const std::string &function, const std::string &fileName, bool removeFile);
	void			modelSelectionChangedNotification(std::list<ot::UID> &selectedEntityID, std::list<ot::UID> &selectedVisibleEntityID);
	void			modelItemRenamed(ot::UID entityID, const std::string &newName);
	void			keySequenceActivated(const std::string &keySequence);

	std::map<std::string, bool> getListOfEntityNames();
	std::map<std::string, ot::UID> getEntityNameToIDMap();

	std::string getCommonPropertiesAsJson(const std::list<ot::UID> &entityIDList, bool visibleOnly);
	void        setPropertiesFromJson(const std::list<ot::UID>& entityIDList, std::string props, bool updateEntities, bool itemsVisible);
	void        deleteProperty(const std::list<ot::UID>& entityIDList, const std::string &propertyName);

	bool entitiesNeedUpdate(void);
	void updateEntities(bool itemsVisible);

	void deleteEntity(EntityBase *entity);
	void facetEntity(EntityGeometry *entity, double deflection, bool isHidden, bool notifyViewer);
	double calculateDeflectionFromListOfEntities(std::list<EntityBase *> &entities);
	double calculateDeflectionFromAllEntities(void);
	void refreshAllViews(void);
	void resetAllViews(void);
	void clearSelection(void);
	void getAllGeometryEntities(std::list<EntityGeometry *> &geometryEntities);
	void getAllEntities(std::list<EntityBase *> &entities);
	void addEntityToModel(std::string entityPath, EntityBase *entity, EntityBase *root, bool addVisualizationContainers, std::list<EntityBase *> &newEntities);
	EntityBase *getRootNode(void) { return entityRoot; };

	std::string getGeometryRootName(void) { return "Geometry"; };
	std::string getMaterialRootName(void) { return "Materials"; };
	std::string getParameterRootName(void) { return "Parameters"; };
	std::string getMeshRootName(void) { return "Meshes"; };
	std::string getSolverRootName(void) { return "Solvers"; };
	std::string getScriptsRootName(void) { return "Scripts"; };
	std::string getUnitRootName(void) { return "Units"; };
	std::string getDataCategorizationRootName(void) { return "Data Categorization"; };
	std::string getDatasetRootName() { return "Dataset"; };
	std::string getRMDCategorizationName() { return getDataCategorizationRootName()+"/Campaign Metadata"; };
	std::string getDatasetRMD() { return getDatasetRootName() + "/Campaign Metadata"; }

	void addVisualizationNodeFromFacetData(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], ot::UID modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
										   double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	void addVisualizationNodeFromFacetDataBase(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool textureReflective, ot::UID modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
										       double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ot::UID entityID, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);

	void addAnnotationEntities(std::list<EntityAnnotation *> &errorAnnotations);
	void addVisualizationAnnotationNode(const std::string &name, ot::UID UID, const TreeIcon &treeIcons, bool isHidden,
									    const double edgeColorRGB[3],
										const std::vector<std::array<double, 3>> &points,
										const std::vector<std::array<double, 3>> &points_rgb,
										const std::vector<std::array<double, 3>> &triangle_p1,
									    const std::vector<std::array<double, 3>> &triangle_p2,
									    const std::vector<std::array<double, 3>> &triangle_p3,
									    const std::vector<std::array<double, 3>> &triangle_rgb);
	void addResult1DEntity(const std::string &name, const std::vector<double> &xdata, const std::vector<double> &ydata, const std::vector<double> &ydataIm,
						   const std::string &curveLabel, const std::string &xlabel, const std::string &xunit,
						   const std::string &ylabel, const std::string &yunit);

	void updateObjectFacetsFromDataBase(ot::UID modelEntityID, ot::UID entityID);

	void removeShapesFromVisualization(std::list<ot::UID> &removeFromDisplay);
	void setShapeVisibility(std::list<ot::UID> &visibleEntityIDs, std::list<ot::UID> &hiddenEntityIDs);

	void displayMessage(const std::string &message);
	void reportError(const std::string &message);
	void reportWarning(const std::string &message);
	void reportInformation(const std::string &message);

	void entitiesSelected(const std::string &selectionAction, const std::string &selectionInfo, std::map<std::string, std::string> &options);

	void         setModelStorageVersion(ot::UID version) { modelStorageVersion = version; };
	ot::UID getModelStorageVersion(void) { return modelStorageVersion; };

	void setModified(void);
	void resetModified(void);
	bool getModified(void) { return isModified; };
	void setClearUiOnDelete(bool _clear) { clearUiOnDelete = _clear; }

	void resetToNew(void);

	bool GetDocumentFromEntityID(ot::UID entityID, bsoncxx::builder::basic::document &doc);

	EntityBase *readEntityFromEntityID(EntityBase *parent, ot::UID entityID, std::map<ot::UID, EntityBase *> &entityMap);
	EntityBase *readEntityFromEntityIDandVersion(EntityBase *parent, ot::UID entityID, ot::UID version, std::map<ot::UID, EntityBase *> &entityMap);

	void prefetchDocumentsFromStorage(std::list<ot::UID> &prefetchIds);

	void addVisualizationContainerNode(const std::string &name, ot::UID entityID, bool isEditable);
	void addVisualizationMeshNode(const std::string &name, ot::UID entityID);

	void projectOpen(void);
	void projectSave(const std::string &comment, bool silentlyCreateBranch);

	void detachAllViewer(void);

	void updatePropertyGrid(void);

	void enableQueuingHttpRequests(bool flag);

	void modelChangeOperationCompleted(const std::string &description);

	void uiIsAvailable(void);

	MicroserviceNotifier *getNotifier(void);

	std::string getCurrentModelVersion(void);
	void activateVersion(const std::string &version);
	void addEntitiesToModel(std::list<ot::UID> &topologyEntityIDList, std::list<ot::UID> &topologyEntityVersionList, std::list<bool> &topologyEntityForceVisible, std::list<ot::UID> &dataEntityIDList, std::list<ot::UID> &dataEntityVersionList, std::list<ot::UID> &dataEntityParentList, const std::string &description, bool saveModel);
	void addGeometryOperation(ot::UID geomEntityID, ot::UID geomEntityVersion, const std::string &geomEntityName, std::list<ot::UID> &dataEntityIDList, std::list<ot::UID> &dataEntityVersionList, std::list<ot::UID> &dataEntityParentList, std::list<std::string> &childrenList, const std::string &description);
	void deleteEntitiesFromModel(std::list<std::string> &entityNameList, bool saveModel);
	void updateVisualizationEntity(ot::UID visEntityID, ot::UID visEntityVersion, ot::UID binaryDataItemID, ot::UID binaryDataItemVersion);
	void updateGeometryEntity(ot::UID geomEntityID, ot::UID brepEntityID, ot::UID brepEntityVersion, ot::UID facetsEntityID, ot::UID facetsEntityVersion, bool overrideGeometry, const std::string &properties, bool updateProperties);
	void updateTopologyEntities(ot::UIDList& topoEntityID, ot::UIDList& topoEntityVersion, const std::string& comment);
	void requestUpdateVisualizationEntity(ot::UID visEntityID);
	std::list<ot::UID> getNewEntityIDs(unsigned long long count);
	std::list<std::string> getListOfFolderItems(const std::string &folder, bool recursive);
	std::list<ot::UID> getIDsOfFolderItemsOfType(const std::string &folder, const std::string &className, bool recursive);
	std::list<EntityBase*> getListOfSelectedEntities(const std::string &typeFilter);
	void addPropertiesToEntities(std::list<ot::UID> &entityIDList, const std::string &propertiesJson);
	void updatePropertiesOfEntities(std::list<ot::UID> &entityIDList, const std::string &propertiesJson);
	void getListOfAllChildEntities(EntityBase* entity, std::list<std::pair<ot::UID, ot::UID>>& childrenEntities);

	void getEntityVersions(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersions);
	void getEntityNames(std::list<ot::UID> &entityIDList, std::list<std::string> &entityNames);
	void getEntityTypes(std::list<ot::UID> &entityIDList, std::list<std::string> &entityTypes);
	std::list<ot::UID> getAllGeometryEntitiesForMeshing(void);
	void getEntityProperties(ot::UID entityID, bool recursive, const std::string &propertyGroupFilter, std::map<ot::UID, std::string> &entityProperties);

	EntityBase *findEntityFromName(const std::string &name);

	void addModalCommand(ModalCommandBase *command);
	void removeModalCommand(ModalCommandBase *command);

	void updateMenuStates(void);

	void hideEntities(std::list<ot::UID> &hiddenEntityIDs);

	void importTableFile(std::string &itemName);

	void setMeshingActive(ot::UID meshEntityID, bool flag);
	bool getMeshingActive(ot::UID meshEntityID);

	std::string checkParentUpdates(std::list<ot::UID> &modifiedEntities);

	int getServiceIDAsInt(void);
	int getSessionCount(void);

	ModelState* getStateManager(void);
	void setStateMangager(ModelState* state);

	void promptResponse(const std::string &type, const std::string &answer, const std::string &parameter1);


private:
	// Methods
	void clearAll(void);
	void setupUIControls();
	void removeUIControls();
	void createVisualizationItems(void);
	void createNewMaterial(void);
	void createNewParameter(void);
	EntityMaterial* createNewMaterial(const std::string &materialName);
	EntityParameter* createNewParameterItem(const std::string &parameterName);
	void deleteSelectedShapes(void);
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
	void showSelectedShapeInformation(void);
	void getFaceCurvatureRadius(const TopoDS_Shape *shape, std::list<double> &faceCurvatureRadius);
	void createFaceAnnotation(const std::list<EntityFaceAnnotationData> &annotations, double r, double g, double b, const std::string &baseName);
	void updateAnnotationGeometry(EntityFaceAnnotation *annotationEntity);
	std::string findMostRecentModelInStorage();
	void addMenuPage(const std::string &menu);
	void addMenuGroup(const std::string &menu, const std::string &group);
	void addMenuSubgroup(const std::string &menu, const std::string &group, const std::string &subgroup);
	void addMenuAction(const std::string &menu, const std::string &group, const std::string &buttonName, const std::string &text, ot::Flags<ot::ui::lockType> &flags, const std::string &iconName, const std::string &iconFolder = std::string("Default"), const std::string &keySequence = std::string(""));
	void addMenuAction(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &buttonName, const std::string &text, ot::Flags<ot::ui::lockType> &flags, const std::string &iconName, const std::string &iconFolder = std::string("Default"), const std::string &keySequence = std::string(""));
	void addMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, const std::string &boxText, bool checked, ot::Flags<ot::ui::lockType> &flags);
	void addMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, const std::string &editLabel, ot::Flags<ot::ui::lockType> &flags);
	bool isUIAvailable(void);
	void processSelectionsForOtherOwners(std::list<ot::UID> &selectedEntities);
	void performSpecialUpdates(EntityBase *entity);
	void performEntityMeshUpdate(EntityMeshTet *entity);
	void performEntityMeshUpdate(EntityMeshCartesian *entity);
	void undoLastOperation(void);
	void redoNextOperation(void);
	void removeAllNonTemplateEntities(void);
	void updateUndoRedoStatus(void);
	void updateModelStateForUndoRedo(void);
	void showByMaterial(void);
	void showMaterialMissing(void);
	void importTableFile(const std::string &fileName, bool removeFile);
	void loadDefaultMaterials(void);
	void findFacesAtIndexFromShape(EntityFaceAnnotation *annotationEntity, std::list<TopoDS_Shape> &facesList, int faceIndex, const TopoDS_Shape *shape);
	void recursiveReplaceEntityName(EntityBase *entity, const std::string &oldName, const std::string &newName, std::list<EntityBase*> &entityList);
	void addChildrenEntitiesToList(EntityGeometry* entity, std::list<std::pair<ot::UID, ot::UID>>& childrenEntities);
	void recursivelyAddEntityInfo(EntityBase *entity, std::map<ot::UID, ot::UID> &entityVersionMap);
	void determineIDandVersionForEntityWithChildren(EntityBase *entity, std::list<ot::UID> &entityInfoIDList, std::list<ot::UID> &entityInfoVersionList);
	void otherServicesUpdate(std::map<std::string, std::list<std::pair<ot::UID, ot::UID>>> otherServicesUpdate, bool itemsVisible);
	void performUpdateVisualizationEntity(std::list<ot::UID> entityIDs, std::list<ot::UID> entityVersions, std::list<ot::UID> brepVersions, std::string owningService);
	bool updateNumericalValues(EntityBase *entity);
	bool evaluateExpressionDouble(const std::string &expression, double &value, ot::UID entityID, const std::string &propertyName);
	void setParameter(const std::string &name, double value, EntityParameter *parameter);
	void removeParameter(const std::string &name);
	void setParameterDependency(std::list<std::string> &parameters, ot::UID entityID, const std::string &propertyName);
	void removeParameterDependency(ot::UID entityID);
	bool checkCircularParameterDependency(EntityParameter *parameter, std::list<std::string> &dependingOnParameters);
	bool checkWhetherParameterDependsOnGivenParameter(EntityParameter *parameter, ot::UID dependingEntityID);
	void replaceParameterByString(EntityParameter *parameter, const std::string &value);
	std::string replaceParameterInExpression(std::string expression, const std::string &parameterName, const std::string &newString);
	bool checkParameterName(const std::string &parameterName);
	bool isValidParameterNameCharacter(char c);
	void sendVersionGraphToUI(std::list<std::tuple<std::string, std::string, std::string>> &versionGraph, const std::string &currentVersion, const std::string &activeBranch);
	void updateVersionGraph(void);
	void setActiveVersionTreeState(void);
	void removeVersionGraphVersions(const std::list<std::string> &versions);
	void addNewVersionTreeStateAndActivate(const std::string &parentVersion, const std::string &newVersion, const std::string &activeBranch, const std::string &description);
	void getIDsOfFolderItemsOfType(EntityContainer *container, const std::string &className, bool recursive, std::list<ot::UID> &itemList);
	void otherOwnersNotification(std::map<std::string, std::list<ot::UID>> ownerEntityListMap);
	size_t getNumberOfVisualizationTriangles(std::list<EntityGeometry *> geometryEntities);
	std::list<EntityBase*> getListOfEntitiesToConsiderForPropertyChange(const std::list<EntityBase*>& entities);
	void getEntityProperties(EntityBase* entity, bool recursive, const std::string& propertyGroupFilter, std::map<ot::UID, std::string>& entityProperties);
	void addTopologyEntitiesToModel(std::list<EntityBase*>& entities, std::list<bool>& forceVisible);
	std::list<ot::UID> RemoveBlockConnections(std::list<EntityBase*>& entityID);
	std::list<EntityBase*> FindTopLevelBlockEntities(std::list<EntityBase*>& entityID);

	// Persistent attributes (need to be stored in data base)
	EntityContainer               *entityRoot;

	ot::UID modelStorageVersion;
	bool anyDataChangeSinceLastWrite;

	// Temporary attributes (need to be rebuilt when the model is loaded)
	std::map<ot::UID, EntityBase *>    entityMap;
	std::map<EntityBase*, bool>	   pendingEntityUpdates;
	
	// Temporary attributes
	std::list<ot::UID>				   selectedModelEntityIDs;
	std::list<ot::UID>				   selectedVisibleModelEntityIDs;
	ot::UID							   visualizationModelID;
	bool						   isModified;
	std::string					   projectName;
	std::string					   projectType;
	std::string					   collectionName;
	std::map<std::string, bool>    uiMenuMap;
	std::map<std::string, bool>    uiGroupMap;
	std::map<std::string, bool>    uiSubGroupMap;
	std::map<std::string, bool>    uiActionMap;
	bool						   clearUiOnDelete;
	std::list<std::string>		   ownersWithSelection;
	bool						   uiCreated;
	bool						   versionGraphCreated;
	std::string					   newTableItemName;
	std::map<ot::UID, bool>		   meshingActive;
	bool						   modelSelectionChangedNotificationInProgress;
	std::map<std::string, std::pair<double, EntityParameter*>>  parameterMap;

	Model() = delete;
	Model(const Model&) = delete;

	// Model state manager
	ModelState *stateManager;

	// Class factory
	ClassFactoryModel classFactory;

	// Flag indicating whether the model is currently shutting down (no more modified messages are sent)
	bool shutdown;

	// List of Modal operations
	std::list<ModalCommandBase *> modalCommands;


};



