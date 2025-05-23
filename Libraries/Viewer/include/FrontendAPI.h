#pragma once

// Viewer header
#include "OldTreeIcon.h"

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTGui/WidgetViewBase.h"
#include "OTGui/PropertyGridCfg.h"

// std header
#include <list>
#include <string>
#include <vector>

namespace ot { class WidgetView; };
namespace ot { class TextEditor; };

class __declspec(dllexport) FrontendAPI {
public:
	static void setInstance(FrontendAPI* _api);
	static FrontendAPI* instance(void);

	FrontendAPI() {};
	virtual ~FrontendAPI() {};

	virtual void createTree(void) {};
	virtual void clearTree(void) {};
	virtual ot::UID addTreeItem(const std::string& treePath, bool editable, bool selectChildren) { return 0; };
	virtual void removeTreeItems(std::list<ot::UID> treeItemIDList) {};
	virtual void selectTreeItem(ot::UID treeItemID) {};
	virtual void selectSingleTreeItem(ot::UID treeItemID) {};
	virtual void expandSingleTreeItem(ot::UID treeItemID) {};
	virtual bool isTreeItemExpanded(ot::UID treeItemID) { return false; };
	virtual void toggleTreeItemSelection(ot::UID treeItemID, bool considerChilds) {};
	virtual void clearTreeSelection(void) {};
	virtual void setTreeItemIcon(ot::UID treeItemID, int iconSize, const std::string& iconName) {};
	virtual void setTreeItemText(ot::UID treeItemID, const std::string& text) {};
	virtual void refreshSelection(void) {};
	virtual void addKeyShortcut(const std::string& keySequence) {};
	virtual void fillPropertyGrid(const ot::PropertyGridCfg& _configuration) {};
	virtual void setDoublePropertyValue(const std::string& _groupName, const std::string& _itemName, double value) {};
	virtual double getDoublePropertyValue(const std::string& _groupName, const std::string& _itemName) { return 0.0; };
	virtual void lockSelectionAndModification(bool flag) {};
	virtual void removeViewer(ot::UID viewerID) {};

	virtual void setCurveDimmed(const std::string& _plotName, ot::UID _entityID, bool _setDimmed) {};

	virtual void closeView(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {};

	virtual bool hasViewFocus(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) { return false; };

	virtual unsigned long long addMenuPage(const std::string& pageName) { return 0; };
	virtual unsigned long long addMenuGroup(unsigned long long menuPageID, const std::string& groupName) { return 0; };
	virtual unsigned long long addMenuSubGroup(unsigned long long _menuGroupID, const std::string& _subGroupName) { return 0; };
	virtual unsigned long long addMenuPushButton(unsigned long long menuGroupID, const std::string& buttonName, const std::string& iconName) { return 0; };
	virtual unsigned long long addMenuPushButton(unsigned long long menuGroupID, const std::string& buttonName, const std::string& iconName, const std::string& keySequence) { return 0; };
	virtual void setMenuPushButtonToolTip(unsigned long long _buttonID, const std::string& _toolTip) {};

	virtual void setCurrentMenuPage(const std::string& _pageName) {};
	virtual std::string getCurrentMenuPage(void) { return std::string(); };

	virtual void removeUIElements(std::list<unsigned long long>& itemIDList) {};
	virtual void removeGraphicsElements(ot::UID _modelID) {};

	virtual void displayText(const std::string& text) {};

	virtual ot::WidgetView* getCurrentView(void) { return nullptr; };

	virtual bool getCurrentViewIsModified(void) { return false; };

	virtual void setCurrentVisualizationTabFromEntityName(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {};
	virtual void setCurrentVisualizationTabFromTitle(const std::string& _tabTitle) {};
	virtual std::string getCurrentVisualizationTabTitle(void) { return ""; };

	virtual void requestSaveForCurrentVisualizationTab(void) {};

	virtual void enableDisableControls(const ot::UIDList& _enabledControls, bool _resetDisabledCounterForEnabledControls, const ot::UIDList& _disabledControls) {};

	virtual void entitiesSelected(ot::serviceID_t replyTo, const std::string& selectionAction, const std::string& selectionInfo, std::list<std::string>& optionNames, std::list<std::string>& optionValues) {};

	virtual void fatalError(const std::string& message) {};

	virtual void rubberbandFinished(ot::serviceID_t creatorId, const std::string& note, const std::string& pointJson, const std::vector<double>& transform) {}

	virtual void updateSettings(const ot::PropertyGridCfg& _config) {};

	virtual void loadSettings(ot::PropertyGridCfg& _config) {};

	virtual void saveSettings(const ot::PropertyGridCfg& _config) {};

	virtual void updateVTKEntity(unsigned long long modelEntityID) {};

	virtual void messageModelService(const std::string& _message) {};

	virtual std::string getOpenFileName(const std::string& _title, const std::string& _path, const std::string& _filters) { return std::string(); };
	virtual std::string getSaveFileName(const std::string& _title, const std::string& _path, const std::string& _filters) { return std::string(); };

private:
	static FrontendAPI*& getInstance(void);
};
