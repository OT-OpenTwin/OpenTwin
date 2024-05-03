#include "UserSettings.h"
#include "AppBase.h"

#include "OTServiceFoundation/SettingsData.h"
#include "OTCore/OTAssert.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"

#include <akAPI/uiAPI.h>
#include <akDialogs/aOptionsDialog.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aComboBoxWidget.h>
#include <akWidgets/aWindow.h>

using namespace ak;

UserSettings * g_userSettings{ nullptr };

SettingsItemOwner::SettingsItemOwner(const SettingsItemOwner& _other)
	: m_item(_other.m_item), m_owner(_other.m_owner), m_uiItem(_other.m_uiItem) {}

SettingsItemOwner::SettingsItemOwner(ot::ServiceBase * _owner, ot::AbstractSettingsItem * _item, ak::aAbstractOptionsItem * _uiItem)
	: m_owner(_owner), m_item(_item), m_uiItem(_uiItem) {}

SettingsItemOwner& SettingsItemOwner::operator = (const SettingsItemOwner& _other) {
	m_owner = _other.m_owner;
	m_item = _other.m_item;
	m_uiItem = _other.m_uiItem;
	return *this;
}

// #########################################################################################################################################

// #########################################################################################################################################

// #########################################################################################################################################

UserSettings * UserSettings::instance(void) {
	if (g_userSettings == nullptr) {
		g_userSettings = new UserSettings;
	}
	return g_userSettings;
}

void UserSettings::showDialog(void) {
	if (m_dialog) { return; }

	m_dialog = new aOptionsDialog(aOptionsDialog::optionsClose, AppBase::instance()->mainWindow());
	m_dialog->setWindowTitle("Open Twin Settings");
	m_dialog->setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	if (m_uiServiceSettings) {
		for (auto g : m_uiServiceSettings->groups()) {
			m_dialog->addGroup(parseFromSettingsGroup(nullptr, g));
		}
	}
	if (m_viewerSettings) {
		for (auto g : m_viewerSettings->groups()) {
			m_dialog->addGroup(parseFromSettingsGroup(nullptr, g));
		}
	}
	for (auto sender : m_serviceToSettingsMap) {
		for (auto g : sender.second->groups()) {
			m_dialog->addGroup(parseFromSettingsGroup(sender.first, g));
		}
	}

	connect(m_dialog, &aOptionsDialog::itemChanged, this, &UserSettings::slotItemChanged);
	m_dialog->showDialog(AppBase::instance()->mainWindow());
	disconnect(m_dialog, &aOptionsDialog::itemChanged, this, &UserSettings::slotItemChanged);

	delete m_dialog;
	m_dialog = nullptr;
}

void UserSettings::showDialog(const QString& _group) {
	if (m_dialog) { return; }

	m_dialog = new aOptionsDialog(aOptionsDialog::optionsClose, AppBase::instance()->mainWindow());
	m_dialog->setWindowTitle("Open Twin Settings");
	m_dialog->setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	if (m_uiServiceSettings) {
		for (auto g : m_uiServiceSettings->groups()) {
			m_dialog->addGroup(parseFromSettingsGroup(nullptr, g));
		}
	}
	if (m_viewerSettings) {
		for (auto g : m_viewerSettings->groups()) {
			m_dialog->addGroup(parseFromSettingsGroup(nullptr, g));
		}
	}
	for (auto sender : m_serviceToSettingsMap) {
		for (auto g : sender.second->groups()) {
			m_dialog->addGroup(parseFromSettingsGroup(sender.first, g));
		}
	}

	connect(m_dialog, &aOptionsDialog::itemChanged, this, &UserSettings::slotItemChanged);

	aOptionsGroup * highlightGroup = m_dialog->findGroupByLogicalName(_group, ":");
	if (highlightGroup) {
		m_dialog->showDialog(highlightGroup, AppBase::instance()->mainWindow());
	}
	else {
		m_dialog->showDialog(AppBase::instance()->mainWindow());
	}
	disconnect(m_dialog, &aOptionsDialog::itemChanged, this, &UserSettings::slotItemChanged);

	delete m_dialog;
	m_dialog = nullptr;
}

void UserSettings::clear(void) {
	if (m_dialog) { OTAssert(0, "Can not clear user settings while dialog is active"); return; }
	if (m_viewerSettings) { delete m_viewerSettings; m_viewerSettings = nullptr; }
	for (auto serviceData : m_serviceToSettingsMap) { delete serviceData.second; }
	m_serviceToSettingsMap.clear();
	m_uiToOwnerMap.clear();
}

void UserSettings::addFromService(ot::ServiceBase * _sender, ot::JsonDocument& _document) {
	QString lastSelection;
	if (m_dialog) {
		lastSelection = m_dialog->lastSelectedGroupName();
	}
	auto oldData = m_serviceToSettingsMap.find(_sender);
	if (oldData != m_serviceToSettingsMap.end()) {
		eraseRootGroups(oldData->second);
		delete oldData->second;
		m_serviceToSettingsMap.erase(_sender);
	}
	ot::SettingsData * serviceSettings = ot::SettingsData::parseFromJsonDocument(_document);
	if (serviceSettings) {
		m_serviceToSettingsMap.insert_or_assign(_sender, serviceSettings);
		if (m_dialog) {
			for (auto g : serviceSettings->groups()) { m_dialog->addGroup(parseFromSettingsGroup(_sender, g)); }
			m_dialog->selectGroupByLogicalName(lastSelection);
		}
	}
}

void UserSettings::updateUiServiceSettings(ot::SettingsData * _data) {
	QString lastSelection;
	if (m_dialog) {
		lastSelection = m_dialog->lastSelectedGroupName();
	}
	eraseRootGroups(_data);
	if (m_uiServiceSettings) { delete m_uiServiceSettings; }
	m_uiServiceSettings = _data;
	if (m_dialog) { 
		for (auto g : _data->groups()) { m_dialog->addGroup(parseFromSettingsGroup(nullptr, g)); }
		m_dialog->selectGroupByLogicalName(lastSelection);
	}
}

void UserSettings::updateViewerSettings(ot::SettingsData * _data) {
	QString lastSelection;
	if (m_dialog) {
		lastSelection = m_dialog->lastSelectedGroupName();
	}
	eraseRootGroups(_data);
	if (m_viewerSettings) { delete m_viewerSettings; }
	m_viewerSettings = _data;
	if (m_dialog) {
		for (auto g : _data->groups()) { m_dialog->addGroup(parseFromSettingsGroup(nullptr, g)); }
		m_dialog->selectGroupByLogicalName(lastSelection);
	}
}

void UserSettings::initializeData(void) {
	ot::SettingsItemSelection* cs = dynamic_cast<ot::SettingsItemSelection*>(m_uiServiceSettings->findItemByLogicalName("General:Appearance:ColorStyle"));
	if (!cs) {
		OT_LOG_E("Item cast failed");
		return;
	}
	cs->setPossibleSelection(ot::GlobalColorStyle::instance().getAvailableStyleNames());
	cs->setSelectedValue(ot::GlobalColorStyle::instance().getCurrentStyleName());
}

// #######################################################################################

// Slots

void UserSettings::slotItemChanged(ak::aAbstractOptionsItem * _item) {
	if (_item == nullptr) { OTAssert(0, "Slot called with nullptr"); return; }
	QString itemName = _item->logicalName();
	auto owner = m_uiToOwnerMap.find(_item);
	if (owner == m_uiToOwnerMap.end()) { OTAssert(0, "Owner data for options item not found"); return; }
	switch (owner->second.item()->type())
	{
	case ot::AbstractSettingsItem::InfoText:
	{
		OTAssert(0, "The info text item should not Q_EMIT any signals");
		return;
	}
	break;
	case ot::AbstractSettingsItem::String:
	{
		ot::SettingsItemString * actualItem = dynamic_cast<ot::SettingsItemString *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemLineEdit * actualUiItem = dynamic_cast<aOptionsItemLineEdit *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setValue(actualUiItem->text().toStdString());
	}
		break;
	case ot::AbstractSettingsItem::Integer:
	{
		ot::SettingsItemInteger * actualItem = dynamic_cast<ot::SettingsItemInteger *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemSpinBox * actualUiItem = dynamic_cast<aOptionsItemSpinBox *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setValue(actualUiItem->value());
	}
	break;
	case ot::AbstractSettingsItem::Double:
	{
		ot::SettingsItemDouble * actualItem = dynamic_cast<ot::SettingsItemDouble *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemDoubleSpinBox * actualUiItem = dynamic_cast<aOptionsItemDoubleSpinBox *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setValue(actualUiItem->value());
	}
	break;
	case ot::AbstractSettingsItem::Boolean:
	{
		ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemCheckBox * actualUiItem = dynamic_cast<aOptionsItemCheckBox *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setValue(actualUiItem->isChecked());
		break;
	}
	case ot::AbstractSettingsItem::Selection:
	{
		ot::SettingsItemSelection * actualItem = dynamic_cast<ot::SettingsItemSelection *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemComboButton * actualUiItem = dynamic_cast<aOptionsItemComboButton *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setSelectedValue(actualUiItem->text().toStdString());
	}
		break;
	case ot::AbstractSettingsItem::EditableSelection:
	{
		ot::SettingsItemEditableSelection * actualItem = dynamic_cast<ot::SettingsItemEditableSelection *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemComboBox * actualUiItem = dynamic_cast<aOptionsItemComboBox *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setSelectedValue(actualUiItem->text().toStdString());
	}
		break;
	case ot::AbstractSettingsItem::ListSelection:
	{
		ot::SettingsItemListSelection * actualItem = dynamic_cast<ot::SettingsItemListSelection *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemList * actualUiItem = dynamic_cast<aOptionsItemList *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		std::list<std::string> selection;
		for (auto itm : actualUiItem->currentSelection()) { selection.push_back(itm.toStdString()); }
		actualItem->setSelectedValues(selection);
	}
	break;
	case ot::AbstractSettingsItem::Color:
	{
		ot::SettingsItemColor * actualItem = dynamic_cast<ot::SettingsItemColor *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemColorEdit * actualUiItem = dynamic_cast<aOptionsItemColorEdit *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		ak::aColor color = actualUiItem->color();
		actualItem->setValue(ot::Color(color.r(), color.g(), color.b(), color.a()));
	}
	break;
	case ot::AbstractSettingsItem::DirectorySelect:
	{
		ot::SettingsItemDirectorySelect * actualItem = dynamic_cast<ot::SettingsItemDirectorySelect *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemDirectorySelect * actualUiItem = dynamic_cast<aOptionsItemDirectorySelect *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setDirectory(actualUiItem->selectedDirectory().toStdString());
	}
	break;
	case ot::AbstractSettingsItem::FileSelectSave:
	{
		ot::SettingsItemFileSelectSave * actualItem = dynamic_cast<ot::SettingsItemFileSelectSave *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemFileSelectSave * actualUiItem = dynamic_cast<aOptionsItemFileSelectSave *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setFile(actualUiItem->selectedFile().toStdString());
	}
	break;
	case ot::AbstractSettingsItem::FileSelectOpen:
	{
		ot::SettingsItemFileSelectOpen * actualItem = dynamic_cast<ot::SettingsItemFileSelectOpen *>(owner->second.item());
		if (actualItem == nullptr) { OTAssert(0, "Item cast failed"); return; }
		aOptionsItemFileSelectOpen * actualUiItem = dynamic_cast<aOptionsItemFileSelectOpen *>(_item);
		if (actualUiItem == nullptr) { OTAssert(0, "UI item cast failed"); return; }
		actualItem->setFile(actualUiItem->selectedFile().toStdString());
	}
	break;
	default:
		OTAssert(0, "Unknown item type");
		return;
	}
	if (owner->second.owner()) { AppBase::instance()->settingsChanged(owner->second.owner(), owner->second.item()); }
	else { uiServiceSettingsChanged(owner->second.item()); }
}

// #######################################################################################

// Private member

void UserSettings::uiServiceSettingsChanged(ot::AbstractSettingsItem * _item) {
	std::string itemName = _item->logicalName();

	// ColorStyle
	if (itemName == "General:Appearance:ColorStyle") {
		ot::SettingsItemSelection* actualItem = dynamic_cast<ot::SettingsItemSelection*>(_item);
		if (!actualItem) {
			OT_LOG_E("Item cast failed");
			return;
		}

		if (ot::GlobalColorStyle::instance().getCurrentStyleName() == actualItem->selectedValue()) return;

		if (!ot::GlobalColorStyle::instance().hasStyle(actualItem->selectedValue())) {
			OT_LOG_E("ColorStyle \"" + actualItem->selectedValue() + "\" not found");
			return;
		}

		ot::GlobalColorStyle::instance().setCurrentStyle(actualItem->selectedValue());
		return;
	}

	// Handle viewer settings
	AppBase::instance()->viewerSettingsChanged(_item);	
}

void UserSettings::eraseRootGroups(ot::SettingsData * _data) {
	if (m_dialog && _data) {
		for (auto g : _data->groups()) { m_dialog->removeGroup(g->name().c_str()); }
	}
}

ak::aOptionsGroup * UserSettings::parseFromSettingsGroup(ot::ServiceBase * _owner, ot::SettingsGroup * _group) {
	aOptionsGroup * newGroup = new aOptionsGroup(_group->name().c_str(), _group->title().c_str());
	for (auto sg : _group->subgroups()) {
		newGroup->addGroup(parseFromSettingsGroup(_owner, sg));
	}
	for (auto itm : _group->items()) {
		aAbstractOptionsItem * item = parseFromSettingsItem(itm);
		if (item) {
			if (!itm->description().empty()) {
				item->setExplanation(itm->description().c_str());
			}
			newGroup->addItem(item);
			SettingsItemOwner ownerData(_owner, itm, item);
			m_uiToOwnerMap.insert_or_assign(item, ownerData);
		}
	}
	return newGroup;
}

ak::aAbstractOptionsItem * UserSettings::parseFromSettingsItem(ot::AbstractSettingsItem * _item) {
	if (_item->isVisible()) {
		switch (_item->type())
		{
		case ot::AbstractSettingsItem::InfoText:
		{
			ot::SettingsItemInfoText * actualItem = dynamic_cast<ot::SettingsItemInfoText *>(_item);
			if (actualItem) {
				return new aOptionsItemInfoText(actualItem->name().c_str(), actualItem->title().c_str());
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::Boolean:
		{
			ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(_item);
			if (actualItem) {
				return new aOptionsItemCheckBox(actualItem->name().c_str(), actualItem->title().c_str(), actualItem->value());
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::String:
		{
			ot::SettingsItemString * actualItem = dynamic_cast<ot::SettingsItemString *>(_item);
			if (actualItem) {
				aOptionsItemLineEdit * newItem = new aOptionsItemLineEdit(actualItem->name().c_str(), actualItem->title().c_str(), actualItem->value().c_str());
				if (!actualItem->inputHint().empty()) {
					newItem->lineEdit()->setPlaceholderText(actualItem->inputHint().c_str());				
				}
				return newItem;
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::Integer:
		{
			ot::SettingsItemInteger * actualItem = dynamic_cast<ot::SettingsItemInteger *>(_item);
			if (actualItem) {
				return new aOptionsItemSpinBox(actualItem->name().c_str(), actualItem->title().c_str(), actualItem->value(), actualItem->minValue(), actualItem->maxValue());
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::Double:
		{
			ot::SettingsItemDouble * actualItem = dynamic_cast<ot::SettingsItemDouble *>(_item);
			if (actualItem) {
				return new aOptionsItemDoubleSpinBox(actualItem->name().c_str(), actualItem->title().c_str(), actualItem->value(), actualItem->minValue(), actualItem->maxValue(), actualItem->decimalPlaces());
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::Selection:
		{
			ot::SettingsItemSelection * actualItem = dynamic_cast<ot::SettingsItemSelection *>(_item);
			if (actualItem) {
				QStringList possibleSelection;
				for (auto ps : actualItem->possibleSelection()) { possibleSelection.push_back(ps.c_str()); }
				return new aOptionsItemComboButton(actualItem->name().c_str(), actualItem->title().c_str(), possibleSelection, actualItem->selectedValue().c_str());
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::EditableSelection:
		{
			ot::SettingsItemEditableSelection * actualItem = dynamic_cast<ot::SettingsItemEditableSelection *>(_item);
			if (actualItem) {
				QStringList possibleSelection;
				for (auto ps : actualItem->possibleSelection()) { possibleSelection.push_back(ps.c_str()); }
				aOptionsItemComboBox * newItem = new aOptionsItemComboBox(actualItem->name().c_str(), actualItem->title().c_str(), possibleSelection, actualItem->selectedValue().c_str());
				if (!actualItem->inputHint().empty()) {
					newItem->comboBox()->lineEdit()->setPlaceholderText(actualItem->inputHint().c_str());
				}
				return newItem;
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::ListSelection:
		{
			ot::SettingsItemListSelection * actualItem = dynamic_cast<ot::SettingsItemListSelection *>(_item);
			if (actualItem) {
				QStringList possibleSelection;
				for (auto ps : actualItem->possibleSelection()) { possibleSelection.push_back(ps.c_str()); }
				QStringList currentSelection;
				for (auto cs : actualItem->selectedValues()) { possibleSelection.push_back(cs.c_str()); }
				return new aOptionsItemList(actualItem->name().c_str(), actualItem->title().c_str(), possibleSelection, currentSelection);
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::Color:
		{
			ot::SettingsItemColor * actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
			if (actualItem) {
				
				aOptionsItemColorEdit * newItem = new aOptionsItemColorEdit(actualItem->name().c_str(), actualItem->title().c_str(),
					ak::aColor(actualItem->value().r(), actualItem->value().g(), actualItem->value().b(), actualItem->value().a()));
				newItem->setEditAlphaChannel(true);
				return newItem;
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::DirectorySelect:
		{
			ot::SettingsItemDirectorySelect * actualItem = dynamic_cast<ot::SettingsItemDirectorySelect *>(_item);
			if (actualItem) {
				aOptionsItemDirectorySelect * newItem = new aOptionsItemDirectorySelect(actualItem->name().c_str(), actualItem->title().c_str(), actualItem->directory().c_str());
				if (!actualItem->inputHint().empty()) {
					newItem->lineEdit()->setPlaceholderText(actualItem->inputHint().c_str());
				}
				return newItem;
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::FileSelectOpen:
		{
			ot::SettingsItemFileSelectOpen * actualItem = dynamic_cast<ot::SettingsItemFileSelectOpen *>(_item);
			if (actualItem) {
				aOptionsItemFileSelectOpen * newItem = new aOptionsItemFileSelectOpen(actualItem->name().c_str(), actualItem->title().c_str(), actualItem->file().c_str());
				if (!actualItem->inputHint().empty()) {
					newItem->lineEdit()->setPlaceholderText(actualItem->inputHint().c_str());
				}
				return newItem;
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		case ot::AbstractSettingsItem::FileSelectSave:
		{
			ot::SettingsItemFileSelectSave * actualItem = dynamic_cast<ot::SettingsItemFileSelectSave *>(_item);
			if (actualItem) {
				aOptionsItemFileSelectSave * newItem = new aOptionsItemFileSelectSave(actualItem->name().c_str(), actualItem->title().c_str(), actualItem->file().c_str());
				if (!actualItem->inputHint().empty()) {
					newItem->lineEdit()->setPlaceholderText(actualItem->inputHint().c_str());
				}
				return newItem;
			}
			OTAssert(0, "Cast item failed");
			break;
		}
		default:
			OTAssert(0, "Unknown item type provided");
		}
	}
	return nullptr;
}

UserSettings::UserSettings() : m_dialog(nullptr), m_uiServiceSettings(nullptr), m_viewerSettings(nullptr) {
	m_uiServiceSettings = new ot::SettingsData("uiServiceSettings", "1.0");
	ot::SettingsGroup * gGeneral        = m_uiServiceSettings->addGroup("General", "General");
	ot::SettingsGroup * gAppearance     = gGeneral->addSubgroup("Appearance", "Appearance");

	std::list<std::string> opt;
	std::string c;

	if (ot::GlobalColorStyle::instance().hasStyle(OT_COLORSTYLE_NAME_Bright)) {
		opt.push_back(OT_COLORSTYLE_NAME_Bright);
		c = OT_COLORSTYLE_NAME_Bright;
	}
	if (ot::GlobalColorStyle::instance().hasStyle(OT_COLORSTYLE_NAME_Dark)) {
		opt.push_back(OT_COLORSTYLE_NAME_Dark);
		if (c.empty()) c = OT_COLORSTYLE_NAME_Dark;
	}
	if (ot::GlobalColorStyle::instance().hasStyle(OT_COLORSTYLE_NAME_Blue)) {
		opt.push_back(OT_COLORSTYLE_NAME_Blue);
		if (c.empty()) c = OT_COLORSTYLE_NAME_Blue;
	}

	ot::AbstractSettingsItem* iColor = new ot::SettingsItemSelection("ColorStyle", "Color Style", opt, c);
	gAppearance->addItem(iColor);
}
