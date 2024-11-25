#pragma once

#include <list>
#include <vector>
#include <string>
#include <map>

#include "Types.h"
#include "Geometry.h"

#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/MessageDialogCfg.h"

class MicroserviceNotifier
{
public:
	MicroserviceNotifier() {};
	virtual ~MicroserviceNotifier() {};

	// These functions needs to be executed synchronously
	void requestFileForReading(const std::string &dialogTitle, const std::string &fileMask, const std::string &subsequentAction, int siteID);

	// All other actions can be executed asynchronously
	void fillPropertyGrid(const ot::PropertyGridCfg& _configuration);
	void addMenuPage(const std::string &pageName);
	void addMenuGroup(const std::string &pageName, const std::string &groupName);
	void addMenuSubGroup(const std::string &pageName, const std::string &groupName, const std::string &subGroupName);
	void addMenuPushButton(const std::string &pageName, const std::string &groupName, const std::string &buttonName, const std::string &text, ot::LockTypeFlags &flags, const std::string &iconName, const std::string &iconFolder = std::string("Default"), const std::string &keySequence = std::string(""));
	void addMenuPushButton(const std::string &pageName, const std::string &groupName, const std::string &subgroupName, const std::string &buttonName, const std::string &text, ot::LockTypeFlags &flags, const std::string &iconName, const std::string &iconFolder = std::string("Default"), const std::string &keySequence = std::string(""));
	void addMenuCheckBox(const std::string &pageName, const std::string &groupName, const std::string &subGroupName, const std::string &boxName, const std::string &boxText, bool checked, ot::LockTypeFlags &flags);
	void addMenuLineEdit(const std::string &pageName, const std::string &groupName, const std::string &subGroupName, const std::string &editName, const std::string &editText, const std::string &editLabel, ot::LockTypeFlags &flags);
	void addShortcut(const std::string &keySequence);
	//Not existing in uiComponent
	void setMenuCheckBox(const std::string &pageName, const std::string &groupName, const std::string &subGroupName, const std::string &boxName, bool checked);
	//Not existing in uiComponent
	void setMenuLineEdit(const std::string &pageName, const std::string &groupName, const std::string &subGroupName, const std::string &editName, const std::string &editText, bool error);
	void removeUIElements(const std::string &type, std::list<std::string> &itemList);
	//Not existing in uiComponent
	void activateMenuTab(const std::string &pageName);
	//Not existing in uiComponent
	void enableDisableControls(std::list<std::string> &enabled, std::list<std::string> &disabled);
	//setControlToolTip in uiComponent ?
	void setToolTip(const std::string &item, const std::string &text);

	void displayMessage(const std::string &message);
	//Not existing in uiComponent
	void reportError(const std::string &message);
	//Not existing in uiComponent
	void reportWarning(const std::string &message);
	//Not existing in uiComponent
	void reportInformation(const std::string &message);
	//Not existing in uiComponent
	void resetAllViews(ot::UID visualizationModelID);

	void refreshAllViews(ot::UID visualizationModelID);
	//Not existing in uiComponent
	void clearSelection(ot::UID visualizationModelID);
	void refreshSelection(ot::UID visualizationModelID);
	//Not existing in uiComponent
	void selectObject(ot::UID visualizationModelID, ot::UID entityID);
	//Not existing in uiComponent
	void promptChoice(const std::string& _message, ot::MessageDialogCfg::BasicIcon _icon, ot::MessageDialogCfg::BasicButtons _buttons, const std::string& _promptResponse, const std::string& _parameter1);
	//Not existing in uiComponent
	void addVisualizationNodeFromFacetData(ot::UID visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], ot::UID modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
		double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	void addVisualizationNodeFromFacetDataBase(ot::UID visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool textureReflective, ot::UID modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
		double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ot::UID entityID, ot::UID entityVersion, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);
	void addVisualizationContainerNode(ot::UID visModelID, const std::string &treeName, ot::UID modelEntityID, const TreeIcon &treeIcons, bool isEditable);
	void addVisualizationAnnotationNode(ot::UID visModelID, const std::string &name, ot::UID UID, const TreeIcon &treeIcons, bool isHidden,
		const double edgeColorRGB[3],
		const std::vector<std::array<double, 3>> &points,
		const std::vector<std::array<double, 3>> &points_rgb,
		const std::vector<std::array<double, 3>> &triangle_p1,
		const std::vector<std::array<double, 3>> &triangle_p2,
		const std::vector<std::array<double, 3>> &triangle_p3,
		const std::vector<std::array<double, 3>> &triangle_rgb);

	void updateObjectColor(ot::UID visModelID, ot::UID modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool textureReflective);
	void updateObjectFacetsFromDataBase(ot::UID visModelID, ot::UID modelEntityID, ot::UID entityID, ot::UID entityVersion);

	void removeShapesFromVisualization(ot::UID visualizationModelID, std::list<ot::UID> entityID);
	void setShapeVisibility(ot::UID visualizationModelID, std::list<ot::UID> &visibleEntityIDs, std::list<ot::UID> &hiddenEntityIDs);
	void hideEntities(ot::UID visualizationModelID, std::list<ot::UID> &hiddenEntityIDs);
	void setTreeStateRecording(ot::UID visualizationModelID, bool flag);
	//until here
	void enterEntitySelectionMode(ot::UID visualizationModelID, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage, const std::map<std::string, std::string> &options);
	//Not existing in uiComponent
	void isModified(ot::UID visualizationModelID, bool modifiedState);
	//Not existing in uiComponent
	void enableQueuingHttpRequests(bool flag);
	//Not existing in uiComponent
	void queuedHttpRequestToUI(ot::JsonDocument &doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds);

	std::string sendMessageToService(bool queue, const std::string &owner, ot::JsonDocument &doc);
	
	//Not existing in uiComponent
	bool isUIAvailable(void);
};
