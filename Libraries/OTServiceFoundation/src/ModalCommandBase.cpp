#include "OTServiceFoundation/ModalCommandBase.h"
#include "OTServiceFoundation/ApplicationBase.h"

ot::ModalCommandBase::ModalCommandBase(ot::ApplicationBase *app, const std::string &actionMnuID, const std::string &actionBtnID) 
	: application(app),
	  actionMenuID(actionMnuID),
	  actionButtonID(actionBtnID)
{
	application->addModalCommand(this);
}

ot::ModalCommandBase::~ModalCommandBase()
{
	application->removeModalCommand(this);
	application->enableMessageQueuing(application->getUiComponent()->getServiceName(), true);

	// Remove all UI elements from the modal command tab
	std::list<std::string> objectNameList;
	for (auto item : uiActionMap) objectNameList.push_back(item.first);
	for (auto item : uiSubGroupMap) objectNameList.push_back(item.first);
	for (auto item : uiGroupMap) objectNameList.push_back(item.first);
	for (auto item : uiMenuMap) objectNameList.push_back(item.first);
	application->getUiComponent()->removeUIElements(objectNameList);

	// Enable the action button
	application->getUiComponent()->setControlState(actionButtonID, true);

	// Update the menu states and send everything
	application->modelSelectionChanged();
	application->enableMessageQueuing(application->getUiComponent()->getServiceName(), false);

	// Activate the previous tab from which the command was originally started
	application->getUiComponent()->activateMenuPage(actionMenuID);
}

void ot::ModalCommandBase::setupUI(void)
{
	// Add the specific commands
	application->enableMessageQueuing(application->getUiComponent()->getServiceName(), true);

	LockTypeFlags modelRead;
	modelRead.setFlag(LockModelRead);

	LockTypeFlags modelWrite;
	modelWrite.setFlag(LockModelWrite);

	std::string mainTab = initializeAndCreateUI(modelRead, modelWrite);

	application->getUiComponent()->setControlState(actionButtonID, false);
	application->getUiComponent()->activateMenuPage(mainTab);

	application->modelSelectionChanged();

	application->enableMessageQueuing(application->getUiComponent()->getServiceName(), false);
}

void ot::ModalCommandBase::addMenuPage(const std::string &menu)
{
	application->getUiComponent()->addMenuPage(menu);
	uiMenuMap[menu] = true;
}

void ot::ModalCommandBase::addMenuGroup(const std::string &menu, const std::string &group)
{
	application->getUiComponent()->addMenuGroup(menu, group);
	uiGroupMap[menu + ":" + group] = true;
}

void ot::ModalCommandBase::addMenuSubgroup(const std::string &menu, const std::string &group, const std::string &subgroup)
{
	application->getUiComponent()->addMenuSubGroup(menu, group, subgroup);
	uiSubGroupMap[menu + ":" + group + ":" + subgroup] = true;
}

void ot::ModalCommandBase::addMenuAction(const std::string &menu, const std::string &group, const std::string &buttonName, const std::string &text, const LockTypeFlags& flags, const std::string &iconName, const std::string &iconFolder)
{
	application->getUiComponent()->addMenuButton(menu, group, buttonName, text, flags, iconName, iconFolder);
	uiActionMap[menu + ":" + group + ":" + buttonName] = true;
}

void ot::ModalCommandBase::addMenuAction(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &buttonName, const std::string &text, const LockTypeFlags& flags, const std::string &iconName, const std::string &iconFolder)
{
	application->getUiComponent()->addMenuButton(menu, group, subgroup, buttonName, text, flags, iconName, iconFolder);
	uiActionMap[menu + ":" + group + ":" + subgroup + ":" + buttonName] = true;
}

void ot::ModalCommandBase::addMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, const std::string &boxText, bool checked, const LockTypeFlags& flags)
{
	application->getUiComponent()->addMenuCheckbox(menu, group, subgroup, boxName, boxText, checked, flags);
	uiActionMap[menu + ":" + group + ":" + subgroup + ":" + boxName] = true;
}

void ot::ModalCommandBase::addMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, const std::string &editLabel, const LockTypeFlags& flags)
{
	application->getUiComponent()->addMenuLineEdit(menu, group, subgroup, editName, editLabel, editText, flags);
	uiActionMap[menu + ":" + group + ":" + subgroup + ":" + editName] = true;
}

void ot::ModalCommandBase::setMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, bool checked)
{
	application->getUiComponent()->setCheckboxValues(menu + ":" + group + ":" + subgroup + ":"+ boxName, checked);
}

void ot::ModalCommandBase::setMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, bool error)
{
	application->getUiComponent()->setLineEditValues(menu + ":" + group + ":" + subgroup + ":"+ editName, editText, error);
}
