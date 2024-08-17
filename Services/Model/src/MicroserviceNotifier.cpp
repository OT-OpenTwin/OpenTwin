// Model header
#include "stdafx.h"

#include "MicroserviceNotifier.h"
#include "MicroserviceAPI.h"
#include "Application.h"

#include "Model.h"
#include "Types.h"

// OpenTwin header
#include "DataBase.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/Msg.h"

// std header
#include <array>
#include <fstream>
#include <cstdio>

void MicroserviceNotifier::requestFileForReading(const std::string &dialogTitle, const std::string &fileMask, const std::string &subsequentFunction, int siteID)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_RequestFileForReading);
	inDoc.AddMember(OT_ACTION_PARAM_FILE_LoadContent, false, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, rapidjson::Value(dialogTitle.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_FILE_Mask, rapidjson::Value(fileMask.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, rapidjson::Value(subsequentFunction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SITE_ID, siteID, inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::fillPropertyGrid(const ot::PropertyGridCfg& _configuration)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_FillPropertyGrid);
	ot::JsonObject cfgObj;
	_configuration.addToJsonObject(cfgObj, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_Config, cfgObj, inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addMenuPage(const std::string &pageName)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_AddMenuPage);
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, rapidjson::Value(pageName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addMenuGroup(const std::string &pageName, const std::string &groupName)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_AddMenuGroup);
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, rapidjson::Value(pageName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, rapidjson::Value(groupName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addMenuSubGroup(const std::string &pageName, const std::string &groupName, const std::string &subGroupName)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_AddMenuSubgroup);
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, rapidjson::Value(pageName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, rapidjson::Value(groupName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName, rapidjson::Value(subGroupName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addMenuPushButton(const std::string &pageName, const std::string &groupName, const std::string &buttonName, const std::string &text, ot::LockTypeFlags &flags, const std::string &iconName, const std::string &iconFolder, const std::string &keySequence)
{
	addMenuPushButton(pageName, groupName, "", buttonName, text, flags, iconName, iconFolder, keySequence);
}

void MicroserviceNotifier::addMenuPushButton(const std::string &pageName, const std::string &groupName, const std::string &subgroupName, const std::string &buttonName, const std::string &text, ot::LockTypeFlags &flags, const std::string &iconName, const std::string &iconFolder, const std::string &keySequence) {
	ot::JsonDocument inDoc;

	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_AddMenuButton, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, ot::JsonString(pageName, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, ot::JsonString(groupName, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName, ot::JsonString(subgroupName, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, ot::JsonString(buttonName, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, ot::JsonString(text, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_IconName, ot::JsonString(iconName, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_IconFolder, ot::JsonString(iconFolder, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, ot::JsonArray(ot::toStringList(flags), inDoc.GetAllocator()), inDoc.GetAllocator());
	if (!keySequence.empty()) {
		inDoc.AddMember(OT_ACTION_PARAM_UI_KeySequence, ot::JsonString(keySequence, inDoc.GetAllocator()), inDoc.GetAllocator());
	}

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addMenuCheckBox(const std::string &pageName, const std::string &groupName, const std::string &subGroupName, const std::string &boxName, const std::string &boxText, bool checked, ot::LockTypeFlags &flags)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_AddMenuCheckbox);
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, rapidjson::Value(pageName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, rapidjson::Value(groupName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName, rapidjson::Value(subGroupName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, rapidjson::Value(boxName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, rapidjson::Value(boxText.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_CheckedState, checked, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, ot::JsonArray(ot::toStringList(flags), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addMenuLineEdit(const std::string &pageName, const std::string &groupName, const std::string &subGroupName, const std::string &editName, const std::string &editText, const std::string &editLabel, ot::LockTypeFlags &flags)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_AddMenuLineEdit);
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, rapidjson::Value(pageName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, rapidjson::Value(groupName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName, rapidjson::Value(subGroupName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, rapidjson::Value(editName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, rapidjson::Value(editText.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText, rapidjson::Value(editLabel.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, ot::JsonArray(ot::toStringList(flags), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addShortcut(const std::string &keySequence) {
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_AddShortcut, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_KeySequence, ot::JsonString(keySequence, inDoc.GetAllocator()), inDoc.GetAllocator());
	
	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::setMenuCheckBox(const std::string &pageName, const std::string &groupName, const std::string &subGroupName, const std::string &boxName, bool checked)
{
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SetCheckboxValues, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, ot::JsonString(pageName + ":" + groupName + ":" + subGroupName + ":" + boxName, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_CheckedState, checked, inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::setMenuLineEdit(const std::string &pageName, const std::string &groupName, const std::string &subGroupName, const std::string &editName, const std::string &editText, bool error)
{
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SetLineEditValues, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, ot::JsonString(pageName + ":" + groupName + ":" + subGroupName + ":" + editName, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, ot::JsonString(editText, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ErrorState, error, inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::activateMenuTab(const std::string &pageName)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_SwitchMenuTab);
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, rapidjson::Value(pageName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::updatePlotEntities(ot::UIDList& entityIDs, ot::UIDList& entityVersions, ot::UID visModelID)
{
	ot::JsonDocument doc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_OBJ_Plot1DPropsChanged);
	doc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visModelID), doc.GetAllocator());
	ot::JsonArray jEntityIDs;
	for (ot::UID id : entityIDs)
	{
		jEntityIDs.PushBack(ot::JsonValue(id), doc.GetAllocator());
	}
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, jEntityIDs,doc.GetAllocator());
	ot::JsonArray jEntityVersions;
	for (ot::UID version : entityVersions)
	{
		jEntityVersions.PushBack(ot::JsonValue(version), doc.GetAllocator());
	}
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, jEntityVersions, doc.GetAllocator());
	Application::instance()->queuedRequestToFrontend(doc);
}

void MicroserviceNotifier::removeUIElements(const std::string &type, std::list<std::string> &itemList)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_RemoveElements);
	//inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectType, rapidjson::Value(type.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	ot::JsonArray list(itemList, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectNames, list, inDoc.GetAllocator());

	Application::instance()->queuedRequestToFrontend(inDoc);
}

void MicroserviceNotifier::enableDisableControls(std::list<std::string> &enabled, std::list<std::string> &disabled)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_EnableDisableControls);
	
	ot::JsonArray elist(enabled, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_EnabledControlsList, elist, inDoc.GetAllocator());
	
	ot::JsonArray dlist(disabled, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_DisabledControlsList, dlist, inDoc.GetAllocator());

	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::setToolTip(const std::string &item, const std::string &text)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_OBJ_SetToolTip);
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, rapidjson::Value(item.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, rapidjson::Value(text.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::displayMessage(const std::string &message)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_DisplayMessage);
	inDoc.AddMember(OT_ACTION_PARAM_MESSAGE, rapidjson::Value(message.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::reportError(const std::string &message)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_ReportError);
	inDoc.AddMember(OT_ACTION_PARAM_MESSAGE, rapidjson::Value(message.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::reportWarning(const std::string &message)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_ReportWarning);
	inDoc.AddMember(OT_ACTION_PARAM_MESSAGE, rapidjson::Value(message.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::reportInformation(const std::string &message)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_ReportInformation);
	inDoc.AddMember(OT_ACTION_PARAM_MESSAGE, rapidjson::Value(message.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::promptChoice(const std::string &message, const std::string &icon, const std::string &options, const std::string &promptResponse, const std::string &parameter1)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_PromptInformation);
	inDoc.AddMember(OT_ACTION_PARAM_MESSAGE, rapidjson::Value(message.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_ICON, rapidjson::Value(("Dialog"+icon).c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_OPTIONS, rapidjson::Value(options.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_SENDER, rapidjson::Value("Model", inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_RESPONSE, rapidjson::Value(promptResponse.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_PARAMETER1, rapidjson::Value(parameter1.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::resetAllViews(ot::UID visualizationModelID)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_Reset);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::clearSelection(ot::UID visualizationModelID)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_ClearSelection);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::refreshSelection(ot::UID visualizationModelID)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_RefreshSelection);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::selectObject(ot::UID visualizationModelID, ot::UID entityID)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_SelectObject);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, rapidjson::Value(entityID), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::refreshAllViews(ot::UID visualizationModelID)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_Refresh);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addVisualizationNodeFromFacetData(ot::UID visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], ot::UID modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
															 double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors,
															 bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_AddNodeFromFacetData);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, rapidjson::Value(treeName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, surfaceColorRGB, 3);
	MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, edgeColorRGB, 3);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, rapidjson::Value(modelEntityID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling, rapidjson::Value(backFaceCulling), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_OffsetFactor, rapidjson::Value(offsetFactor), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, rapidjson::Value(isEditable), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_SelectChildren, rapidjson::Value(selectChildren), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ManageParentVis, rapidjson::Value(manageParentVisibility), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ManageChildVis, rapidjson::Value(manageChildVisibility), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected, rapidjson::Value(showWhenSelected), inDoc.GetAllocator());

	MicroserviceAPI::AddNodeToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Nodes, nodes);
	MicroserviceAPI::AddTriangleToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Triangles, triangles);
	MicroserviceAPI::AddEdgeToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Edges, edges);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Errors, rapidjson::Value(errors.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	MicroserviceAPI::addTreeIconsToJsonDoc(inDoc, treeIcons);

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addVisualizationNodeFromFacetDataBase(ot::UID visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool textureReflective, ot::UID modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
																 double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ot::UID entityID, ot::UID entityVersion, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation)
{
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddNodeFromDataBase, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, visModelID, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(treeName, inDoc.GetAllocator()), inDoc.GetAllocator());
	MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, surfaceColorRGB, 3);
	MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, edgeColorRGB, 3);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_MaterialType, ot::JsonString(materialType, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureType, ot::JsonString(textureType, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureReflective, textureReflective, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, modelEntityID, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling, backFaceCulling, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_OffsetFactor, offsetFactor, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, isEditable, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(projectName, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, entityID, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, entityVersion, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_SelectChildren, selectChildren, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ManageParentVis, manageParentVisibility, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ManageChildVis, manageChildVisibility, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected, showWhenSelected, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Transformation, ot::JsonArray(transformation, inDoc.GetAllocator()), inDoc.GetAllocator());

	MicroserviceAPI::addTreeIconsToJsonDoc(inDoc, treeIcons);

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	prefetchIds.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addVisualizationContainerNode(ot::UID visModelID, const std::string &treeName, ot::UID modelEntityID, const TreeIcon &treeIcons, bool isEditable)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_AddContainerNode);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, rapidjson::Value(treeName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, rapidjson::Value(modelEntityID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, rapidjson::Value(isEditable), inDoc.GetAllocator());
	MicroserviceAPI::addTreeIconsToJsonDoc(inDoc, treeIcons);

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::addVisualizationAnnotationNode(ot::UID visModelID, const std::string &name, ot::UID uid, const TreeIcon &treeIcons, bool isHidden,
														  const double edgeColorRGB[3],
														  const std::vector<std::array<double, 3>> &points,
														  const std::vector<std::array<double, 3>> &points_rgb,
														  const std::vector<std::array<double, 3>> &triangle_p1,
														  const std::vector<std::array<double, 3>> &triangle_p2,
														  const std::vector<std::array<double, 3>> &triangle_p3,
														  const std::vector<std::array<double, 3>> &triangle_rgb)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNode);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, rapidjson::Value(name.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_UID, rapidjson::Value(uid), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, rapidjson::Value(isHidden), inDoc.GetAllocator());
	MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, edgeColorRGB,3);
	MicroserviceAPI::AddDoubleArrayVectorToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Points, points);
	MicroserviceAPI::AddDoubleArrayVectorToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Points_Colors, points_rgb);
	MicroserviceAPI::AddDoubleArrayVectorToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p1, triangle_p1);
	MicroserviceAPI::AddDoubleArrayVectorToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p2, triangle_p2);
	MicroserviceAPI::AddDoubleArrayVectorToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p3, triangle_p3);
	MicroserviceAPI::AddDoubleArrayVectorToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_Color, triangle_rgb);
	MicroserviceAPI::addTreeIconsToJsonDoc(inDoc, treeIcons);

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::updateObjectColor(ot::UID visModelID, ot::UID modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool textureReflective)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_OBJ_UpdateColor);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, rapidjson::Value(modelEntityID), inDoc.GetAllocator());
	MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, surfaceColorRGB, 3);
	MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, edgeColorRGB, 3);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_MaterialType, rapidjson::Value(materialType.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureType, rapidjson::Value(textureType.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextureReflective, rapidjson::Value(textureReflective), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::updateObjectFacetsFromDataBase(ot::UID visModelID, ot::UID modelEntityID, ot::UID entityID, ot::UID entityVersion)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_OBJ_UpdateFacetsFromDataBase);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, rapidjson::Value(modelEntityID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, rapidjson::Value((long long)entityID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Version, rapidjson::Value((long long)entityVersion), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	prefetchIds.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::removeShapesFromVisualization(ot::UID visualizationModelID, std::list<ot::UID> entityID)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_OBJ_RemoveShapes);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());
	ot::JsonArray list(entityID, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, list, inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::setTreeStateRecording(ot::UID visualizationModelID, bool flag)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_OBJ_TreeStateRecording);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_State, rapidjson::Value(flag), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::enterEntitySelectionMode(ot::UID visualizationModelID, const std::string &selectionType, bool allowMultipleSelection,
													const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage,
													const std::map<std::string, std::string> &options)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_EnterEntitySelectionMode);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SelectionType, rapidjson::Value(selectionType.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_REPLYTO, Application::instance()->serviceID(), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_AllowMultipleSelection, allowMultipleSelection, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Filter, rapidjson::Value(selectionFilter.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Action, rapidjson::Value(selectionAction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Message, rapidjson::Value(selectionMessage.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::list<std::string> optionNames, optionValues;
	for (auto item : options)
	{
		optionNames.push_back(item.first);
		optionValues.push_back(item.second);
	}

	ot::JsonArray nlist(optionNames, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames, nlist, inDoc.GetAllocator());

	ot::JsonArray vlist(optionValues, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues, vlist, inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::setShapeVisibility(ot::UID visualizationModelID, std::list<ot::UID> &visibleEntityIDs, std::list<ot::UID> &hiddenEntityIDs)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_OBJ_SetShapeVisibility);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());

	ot::JsonArray vlist(visibleEntityIDs, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID_Visible, vlist, inDoc.GetAllocator());

	ot::JsonArray hlist(hiddenEntityIDs, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID_Hidden, hlist, inDoc.GetAllocator());
	
	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::hideEntities(ot::UID visualizationModelID, std::list<ot::UID> &hiddenEntityIDs)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_OBJ_HideEntities);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());

	ot::JsonArray list(hiddenEntityIDs, inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID_Hidden, list, inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::isModified(ot::UID visualizationModelID, bool modifiedState)
{
	ot::JsonDocument inDoc = MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_UI_VIEW_SetModifiedState);
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ModifiedState, rapidjson::Value(modifiedState), inDoc.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->queuedRequestToFrontend(inDoc, prefetchIds);
}

void MicroserviceNotifier::enableQueuingHttpRequests(bool flag)
{
	Application::instance()->enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, flag);
}

void MicroserviceNotifier::queuedHttpRequestToUI(ot::JsonDocument &doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds)
{
	Application::instance()->queuedRequestToFrontend(doc, prefetchIds);
}

bool MicroserviceNotifier::isUIAvailable(void)
{
	return Application::instance()->isUiConnected();
}

std::string MicroserviceNotifier::sendMessageToService(bool queue, const std::string &owner, ot::JsonDocument &doc)
{
	std::string response;
	Application::instance()->sendMessage(queue, owner, doc, response);
	return response;
}

