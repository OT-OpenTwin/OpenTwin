/*
 * ExternalServicesComponent.h
 *
 *  Created on: September 21, 2020
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#pragma once
#include "FileHandler.h"

// Wrapper header
#include <uiServiceTypes.h>				// Model and View types

// Viewer header
#include "ViewerAPI.h"
#include "QtMetaTypes.h"

// C++ header
#include <iostream>
#include <string>					// string
#include <vector>					// vector<T>
#include <list>						// list<T>
#include <array>					// array
#include <map>

// AK header
#include <akCore/aNotifier.h>

// Qt header
#include <qobject.h>				// QObject
#include <qmetatype.h>
#include <qmetaobject.h>				// QMetaObject

// Model header
#include <Geometry.h>

// Rapid JSON
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Flags.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/Owner.h"
#include "OpenTwinCommunication/UiTypes.h"

class WebsocketClient;
class ControlsManager;
class LockManager;
class AppBase;
class KeyboardCommandHandler;
namespace ot { class ServiceBase; }

class ExternalServicesComponent : public QObject, public ak::aNotifier
{
	Q_OBJECT
public:
	enum RequestType { EXECUTE, QUEUE };

	ExternalServicesComponent(AppBase * _owner);
	virtual ~ExternalServicesComponent(void);

	void shutdown(void);

	// ###################################################################################################

	// Getter

	const std::string & gloablSessionServiceURL(void) const { return m_globalSessionServiceURL; }

	const std::string & sessionServiceURL(void) const { return m_sessionServiceURL; }

	const std::string & uiServiceURL(void) const { return m_uiServiceURL; }

	ControlsManager * controlsManager(void) { return m_controlsManager; }

	LockManager * lockManager(void) { return m_lockManager; }

	// ###################################################################################################

	// Configuration

	void setGlobalSessionServiceURL(const std::string& _url) { m_globalSessionServiceURL = _url; }

	void setSessionServiceURL(const std::string& _url) { m_sessionServiceURL = _url; }

	void setUiServiceURL(const std::string& _url) { m_uiServiceURL = _url; }

	void setRelayServiceIsRequired(void);

	void setMessagingRelay(const std::string & _relayAddress);

	// ###################################################################################################

	// UI Element creation
	
	void addMenuPage(ot::ServiceBase * _sender, const std::string &pageName);
	void addMenuGroup(ot::ServiceBase * _sender, const std::string &pageName, const std::string &groupName);
	void addMenuSubgroup(ot::ServiceBase * _sender, const std::string &pageName, const std::string &groupName, const std::string &subgroupName);
	ak::UID addMenuPushButton(ot::ServiceBase * _sender, const std::string & _pageName, const std::string & _groupName, const std::string &_subgroupName, const std::string & _buttonName,
		const std::string & _text, const std::string & _iconName, const std::string & _iconFolder, const ot::Flags<ot::ui::lockType> & _lockFlags);
	void addMenuCheckBox(ot::ServiceBase * _sender, const std::string &pageName, const std::string &groupName, const std::string &subgroupName, const std::string &boxName, const std::string &boxText, bool checked, const ot::Flags<ot::ui::lockType> & _lockFlags);
	void addMenuLineEdit(ot::ServiceBase * _sender, const std::string &pageName, const std::string &groupName, const std::string &subgroupName, const std::string &editName, const std::string &editText, const std::string &editLabel, const ot::Flags<ot::ui::lockType> & _lockFlags);
	KeyboardCommandHandler * addShortcut(ot::ServiceBase * _sender, const std::string& _keySequence);

	void removeUIElements(const std::list<std::string> & _itemList);

	// ###################################################################################################

	// UI Element manipulation

	void fillPropertyGrid(const std::string &settings);

	void setCheckBoxValues(ot::ServiceBase * _sender, const std::string & _controlName, bool checked);
	void setLineEditValues(ot::ServiceBase * _sender, const std::string & _controlName, const std::string &editText, bool error);

	void enableDisableControls(ot::ServiceBase * _sender, std::list<std::string> &enabled, std::list<std::string> &disabled);
	void setTooltipText(ot::ServiceBase * _sender, const std::string &item, const std::string &text);

	void displayInfoMessage(const std::string & _message);
	void displayDebugMessage(const std::string& _message);

	// ###################################################################################################

	// 3D View

	void resetAllViews3D(ModelUIDtype visualizationModelID);
	void refreshAllViews(ModelUIDtype visualizationModelID);
	void clearSelection(ModelUIDtype visualizationModelID);
	void refreshSelection(ModelUIDtype visualizationModelID);
	void selectObject(ModelUIDtype visualizationModelID, ak::UID entityID);
	void addVisualizationNodeFromFacetData(ModelUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3],
		ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles,
		std::list<Geometry::Edge> &edges, std::string &errors, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	void addVisualizationNodeFromFacetDataBase(ViewerUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
		double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ak::UID entityID, ak::UID entityVersion, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);
	void addVisualizationContainerNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool editable);
	void addVisualizationAnnotationNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden,
		const double edgeColorRGB[3],
		const std::vector<std::array<double, 3>> &points,
		const std::vector<std::array<double, 3>> &points_rgb,
		const std::vector<std::array<double, 3>> &triangle_p1,
		const std::vector<std::array<double, 3>> &triangle_p2,
		const std::vector<std::array<double, 3>> &triangle_p3,
		const std::vector<std::array<double, 3>> &triangle_rgb);

	void addVisualizationVis2D3DNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, ak::UID visualizationDataID, ak::UID visualizationDataVersion);
	void updateVisualizationVis2D3DNode(ModelUIDtype visModelID, ModelUIDtype modelEntityID, const std::string &projectName, ak::UID visualizationDataID, ak::UID visualizationDataVersion);

	void addVisualizationAnnotationNodeDataBase(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, ak::UID entityID, ak::UID entityVersion);
	void addVisualizationMeshNodeFromFacetDataBase(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, ak::UID entityID, ak::UID entityVersion);
	void addVisualizationCartesianMeshNode(ModelUIDtype visModelID, const std::string &name, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ, 
												   const std::string &projectName, ak::UID faceListEntityID, ak::UID faceListEntityVersion, ak::UID nodeListEntityID, ak::UID nodeListEntityVersion);
	void visualizationCartesianMeshNodeShowLines(ModelUIDtype visModelID, ModelUIDtype modelEntityID, bool showMeshLines);
	void visualizationTetMeshNodeTetEdges(ModelUIDtype osgModelID, ModelUIDtype modelEntityID, bool displayTetEdges);

	void addVisualizationCartesianMeshItemNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3]);

	void addVisualizationMeshItemNodeFromFacetDataBase(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, ak::UID entityID, ak::UID entityVersion, long long tetEdgesID, long long tetEdgesVersion);
	void addVisualizationPlot1DNode(ak::UID visModelID, const std::string &treeName, ak::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden,
										    const std::string &projectName, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
											bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax, std::list<ak::UID> &curvesID, std::list<ak::UID> &curvesVersions,
										    std::list<std::string> &curvesNames);
	void visualizationPlot1DPropertiesChanged(ak::UID visModelID, ak::UID modelEntityID, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
													  bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax);
	void addVisualizationTextNode(ak::UID visModelID, const std::string &treeName, ak::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden,
								 		  const std::string &projectName, ak::UID textID, ak::UID textVersion);
	void addVisualizationTableNode(ak::UID visModelID, const std::string &treeName, ak::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden,
										   const std::string &projectName, ak::UID tableID, ak::UID tableVersion);


	void visualizationResult1DPropertiesChanged(ModelUIDtype visModelID, ak::UID entityID, ak::UID entityVersion);
	void removeShapesFromVisualization(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> entityID);
	void setTreeStateRecording(ModelUIDtype visualizationModelID, bool flag);
	void setShapeVisibility(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> visibleID, std::list<ModelUIDtype> hiddenID);
	void hideEntities(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> hiddenID);
	void showBranch(ModelUIDtype visualizationModelID, const std::string &branchName);
	void hideBranch(ModelUIDtype visualizationModelID, const std::string &branchName);
	void updateObjectColor(ModelUIDtype visModelID, ModelUIDtype modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective);
	void updateMeshColor(ModelUIDtype visModelID, ModelUIDtype modelEntityID, double colorRGB[3]);
	void updateObjectFacetsFromDataBase(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, unsigned long long entityID, unsigned long long entityVersion);

	void enterEntitySelectionMode(ModelUIDtype visualizationModelID, ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection,
		const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage,
		std::list<std::string> &optionNames, std::list<std::string> &optionValues);
	void freeze3DView(ViewerUIDtype visModelID, bool flag);
	void isModified(ModelUIDtype visualizationModelID, bool modifiedState);

	ModelUIDtype createModel(const std::string& _projectName, const std::string& _collectionName);
	bool deleteModel(ModelUIDtype modelID);

	void setVisualizationModel(ModelUIDtype modelID, ModelUIDtype visualizationModelID);
	ModelUIDtype getVisualizationModel(ModelUIDtype modelID);
	bool isModelModified(ModelUIDtype modelID);
	bool isCurrentModelModified(void);

	std::string getCommonPropertiesAsJson(ModelUIDtype modelID, const std::list<ModelUIDtype> &entityIDList);
	void setPropertiesFromJson(ModelUIDtype modelID, const std::list<ModelUIDtype> &entityIDList, std::string props, bool update, bool itemsVisible);

	// ###################################################################################################

	// Event handling

	virtual void notify(ak::UID _senderId, ak::eventType _event, int _info1, int _info2) override;

	void modelSelectionChangedNotification(ModelUIDtype modelID, std::list<ModelUIDtype> &selectedEntityID, std::list<ModelUIDtype> &selectedVisibleEntityID);

	void itemRenamed(ModelUIDtype modelID, const std::string &newName);

	void propertyGridValueChanged(int itemID);
	void propertyGridValueDeleted(int itemID);

	void executeAction(ModelUIDtype modelID, ModelUIDtype buttonID);

	void entitiesSelected(ModelUIDtype modelID, ot::serviceID_t replyToServiceID, const std::string &selectionAction, const std::string &selectionInfo, std::list<std::string> &optionNames, std::list<std::string> &optionValues);
	
	std::string dispatchAction(rapidjson::Document & _doc, const char * _senderIP);
	
	void prefetchDataThread(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs);

	void contextMenuItemClicked(ot::ServiceBase * _sender, const std::string& _menuName, const std::string& _itemName);

	void contextMenuItemCheckedChanged(ot::ServiceBase * _sender, const std::string& _menuName, const std::string& _itemName, bool _isChecked);

	// ###################################################################################################

	// Messaging

	bool sendHttpRequest(RequestType operation, const std::string &url, rapidjson::Document &doc, std::string &response);
	bool sendHttpRequest(RequestType operation, ot::ServiceOwner_t _service, rapidjson::Document &doc, std::string &response);
	bool sendHttpRequest(RequestType operation, const std::string &url, const std::string &message, std::string &response);
	bool sendRelayedRequest(RequestType operation, const std::string &url, const std::string &json, std::string &response);
	bool sendKeySequenceActivatedMessage(KeyboardCommandHandler * _sender);
	void sendRubberbandResultsToService(ot::serviceID_t _serviceId, const std::string& _note, const std::string& _pointJson, const std::vector<double> &transform);
	void requestUpdateVTKEntity(unsigned long long modelEntityID);
	void activateVersion(const std::string &version);

	
	// ###################################################################################################

	// Project handling

	void openProject(const std::string & projectName, const std::string & collectionName);
	void closeProject(bool saveChanges);
	void saveProject();
	bool projectIsOpened(const std::string &projectName, std::string &projectUser);

	// ###################################################################################################

	// File operations
	std::list<std::string> RequestFileNames(const std::string& dialogTitle, const std::string& fileMask);

	/// <summary>
	/// Opens a file selection dialog. The path of the selected file is send to the service that created the request. Optionally, the content of the file can be send with the http response.
	/// </summary>
	/// <param name="dialogTitle"></param>
	/// <param name="fileMask"> Filters the types of files that are shown in the file dialog. </param>
	/// <param name="subsequentAction"> Info for the requesting service. </param>
	/// <param name="senderURL"></param>
	/// <param name="loadContent"> Option to load the files content and send it as part of the response. </param>
	void requestFileForReading(const std::string &dialogTitle, const std::string &fileMask, const std::string &subsequentAction, const std::string &senderURL, bool loadContent);
	/// <summary>
	/// Opens a SAVE FILE dialog window. The path to the selected file is send back to the requesting service.
	/// </summary>
	/// <param name="dialogTitle"></param>
	/// <param name="fileMask">Filters the types of files that are shown in the file dialog. </param>
	/// <param name="subsequentFunction"></param>
	/// <param name="senderURL"></param>
	void selectFileForStoring(const std::string &dialogTitle, const std::string &fileMask, const std::string &subsequentFunction, const std::string &senderURL);

	void ReadFileContent(const std::string &fileName, std::string &fileContent, unsigned long long &uncompressedDataLength);
	void saveFileContent(const std::string &dialogTitle, const std::string &fileName, const std::string &fileContent, ot::UID uncompressedDataLength);

	// ###################################################################################################

	// Table operations
	void RequestTableSelection(ModelUIDtype visModelID, std::string URL, std::string subsequentFunction);
	void SetColourOfSelectedRange(ModelUIDtype visModelID, ot::Color background);

	//########################################################################################################

	// General purpose communication
	void InformSenderAboutFinishedAction(std::string URL, std::string subsequentFunction);

public slots:
	char *performAction(const char *json, const char *senderIP);
	void queueAction(const char *json, const char *senderIP);
	void deallocateData(const char *data);
	void shutdownAfterSessionServiceDisconnected(void);

private:
	// ###################################################################################################

	// JSON helper functions

	std::string getStringFromDocument(rapidjson::Document &doc, const char *attribute);
	rapidjson::Document BuildJsonDocFromAction(const std::string &action);
	rapidjson::Document BuildJsonDocFromString(std::string json);
	void AddUIDListToJsonDoc(rapidjson::Document &doc, const std::string &itemName, const std::list<ModelUIDtype> &list);
	void AddStringListToJsonDoc(rapidjson::Document &doc, const std::string &itemName, const std::list<std::string> &list);
	void AddUIDVectorToJsonDoc(rapidjson::Document &doc, const std::string &itemName, const std::vector<ModelUIDtype> &vector);

	std::string getReturnJSONFromVector(std::vector<ModelUIDtype> idList);
	std::string getReturnJSONFromString(std::string fileName);

	std::list<ModelUIDtype> getListFromDocument(rapidjson::Document &doc, const std::string &itemName);
	std::list<std::string> getStringListFromDocument(rapidjson::Document &doc, const std::string &itemName);
	double *getDoubleFromDocument(rapidjson::Document &doc, const std::string &itemName);
	std::vector<ModelUIDtype> getVectorFromDocument(rapidjson::Document &doc, const std::string &itemName);
	std::vector<double> getVectorDoubleFromDocument(rapidjson::Document &doc, const std::string &itemName);
	std::vector<int> getVectorIntFromDocument(rapidjson::Document &doc, const std::string &itemName);
	std::vector<std::array<double, 3>> getVectorDoubleArrayFromDocument(rapidjson::Document &doc, const std::string &name);
	TreeIcon getTreeIconsFromDocument(rapidjson::Document &doc);

	void getVectorNodeFromDocument(rapidjson::Document &doc, const std::string &name, std::vector<Geometry::Node> &result);
	void getListTriangleFromDocument(rapidjson::Document &doc, const std::string &name, std::list<Geometry::Triangle> &result);
	void getListEdgeFromDocument(rapidjson::Document &doc, const std::string &name, std::list<Geometry::Edge> &result);

	// ###################################################################################################

	// Viewer helper functions

	void getSelectedModelEntityIDs(std::list<ModelUIDtype> &selected);
	void getSelectedVisibleModelEntityIDs(std::list<ModelUIDtype> &selected);
	
	// ###################################################################################################

	// Private functions

	std::list<std::string> GetAllUserProjects();

	void prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);
	
	void removeServiceFromList(std::vector<ot::ServiceBase *> &list, ot::ServiceBase *service);

	ak::UID getServiceUiUid(ot::ServiceBase * _service);

	ot::ServiceBase * getService(ot::serviceID_t _serviceID);
	ot::ServiceBase * getServiceFromName(const std::string & _serviceName);

	// #################################################################

	std::string										m_globalSessionServiceURL;
	std::string										m_sessionServiceURL;
	std::string										m_uiServiceURL;
	std::string										m_currentSessionID;
	bool											m_isRelayServiceRequired;
	ControlsManager *								m_controlsManager;
	LockManager *									m_lockManager;
	AppBase *										m_owner;

	std::string										m_modelServiceURL;

	std::map<std::string, ak::UID>					m_serviceToUidMap;
	std::map<ot::serviceID_t, ot::ServiceBase *>	m_serviceIdMap;

	std::vector<ot::ServiceBase *>					m_modelViewNotifier;

	std::string										m_uiRelayServiceHTTP;
	std::string										m_uiRelayServiceWS;
	WebsocketClient *								m_websocket;

	bool											m_prefetchingDataCompleted;

	FileHandler										m_fileHandler;

	ExternalServicesComponent() = delete;
	ExternalServicesComponent(ExternalServicesComponent &) = delete;
	ExternalServicesComponent & operator = (ExternalServicesComponent &) = delete;
};

namespace ot {
	
	void startSessionServiceHealthCheck(const std::string& _sessionServiceURL);

	void stopSessionServiceHealthCheck(void);
}

