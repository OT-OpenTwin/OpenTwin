//! @file ApplicationPropertiesManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/ApplicationPropertiesManager.h"

ot::ApplicationPropertiesManager& ot::ApplicationPropertiesManager::instance(void) {
	static ApplicationPropertiesManager g_instance;
	return g_instance;
}

ot::Dialog::DialogResult ot::ApplicationPropertiesManager::showDialog(void) {
	this->clearGarbage();

	PropertyGridCfg gridCfg = this->buildDialogConfiguration();

	// If there are no properties skip showing the dialog.
	if (gridCfg.isEmpty()) {
		return ot::Dialog::Cancel;
	}

	// Show dialog
	PropertyDialogCfg dialogCfg;
	dialogCfg.setGridConfig(gridCfg);

	OTAssert(m_dialog == nullptr, "Dialog already exists");
	m_dialog = new PropertyDialog(dialogCfg);
	m_dialog->setWindowTitle(m_dialogTitle);

	this->connect(m_dialog, &PropertyDialog::propertyChanged, this, &ApplicationPropertiesManager::slotPropertyChanged);
	this->connect(m_dialog, &PropertyDialog::propertyDeleteRequested, this, &ApplicationPropertiesManager::slotPropertyDeleteRequested);

	Dialog::DialogResult result = m_dialog->showDialog();

	this->disconnect(m_dialog, &PropertyDialog::propertyChanged, this, &ApplicationPropertiesManager::slotPropertyChanged);
	this->disconnect(m_dialog, &PropertyDialog::propertyDeleteRequested, this, &ApplicationPropertiesManager::slotPropertyDeleteRequested);

	if (result == Dialog::Ok) {
		for (const Property* prop : m_dialog->getChangedProperties()) {
			m_changedProperties.push_back(prop->createCopyWithParents());
		}
	}

	delete m_dialog;
	m_dialog = nullptr;

	return result;
}

void ot::ApplicationPropertiesManager::clear(void) {
	m_data.clear();
	this->clearGarbage();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ApplicationPropertiesManager::add(const std::string& _owner, const PropertyGridCfg& _config) {
	PropertyGridCfg oldConfig = this->findData(_owner);
	oldConfig.mergeWith(_config, m_propertyReplaceOnMerge);
	m_data.insert_or_assign(_owner, oldConfig);

	// If the dialog is visible we need to update the properties displayed there.
	if (m_dialog) {
		this->updateCurrentDialog();
	}
}

void ot::ApplicationPropertiesManager::setDialogTitle(const QString& _title) {
	m_dialogTitle = _title;
	if (m_dialog) {
		m_dialog->setWindowTitle(_title);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ot::ApplicationPropertiesManager::slotPropertyChanged(const Property* _property) {
	std::string owner;
	Property* cleanedProperty = this->createCleanedSlotProperty(_property, owner);
	OTAssertNullptr(cleanedProperty);
	Q_EMIT propertyChanged(owner, cleanedProperty);
}

void ot::ApplicationPropertiesManager::slotPropertyDeleteRequested(const Property* _property) {
	std::string owner;
	Property* cleanedProperty = this->createCleanedSlotProperty(_property, owner);
	OTAssertNullptr(cleanedProperty);
	Q_EMIT propertyDeleteRequested(owner, cleanedProperty);
}

ot::PropertyGridCfg ot::ApplicationPropertiesManager::findData(const std::string& _owner) {
	const auto& it = m_data.find(_owner);
	if (it == m_data.end()) {
		return PropertyGridCfg();
	}
	OTAssert(it != m_data.end(), "Entry not found");
	return it->second;
}

ot::Property* ot::ApplicationPropertiesManager::createCleanedSlotProperty(const Property* _property, std::string& _owner) {
	ot::Property* newProperty = _property->createCopyWithParents();
	OTAssertNullptr(newProperty);

	m_garbage.push_back(newProperty);

	PropertyGroup* rootGroup = newProperty->getRootGroup();
	if (rootGroup->getChildGroups().empty()) {
		OT_LOG_EA("Data mismatch");
		return newProperty;
	}

	PropertyGroup* newRoot = rootGroup->getChildGroups().front();
	newRoot->setParentGroup(nullptr);
	rootGroup->forgetChildGroup(newRoot);
	_owner = rootGroup->getName();
	delete rootGroup;
	rootGroup = nullptr;

	return newProperty;
}

void ot::ApplicationPropertiesManager::updateCurrentDialog(void) {
	if (!m_dialog) return;

	PropertyDialogCfg dialogCfg;
	dialogCfg.setGridConfig(this->buildDialogConfiguration());

	m_dialog->setupFromConfiguration(dialogCfg);
	m_dialog->setWindowTitle(m_dialogTitle);
}

ot::PropertyGridCfg ot::ApplicationPropertiesManager::buildDialogConfiguration(void) {
	// Build configuration
	PropertyGridCfg gridCfg;

	for (const auto& it : m_data) {
		PropertyGroup* newGroup = new PropertyGroup(it.first);
		for (PropertyGroup* newChild : it.second.getRootGroups()) {
			if (!newChild->isEmpty()) {
				newGroup->addChildGroup(new PropertyGroup(*newChild));
			}
		}

		if (!newGroup->isEmpty()) {
			gridCfg.addRootGroup(newGroup);
		}
		else {
			// Skip empty groups
			delete newGroup;
		}
	}

	return gridCfg;
}

void ot::ApplicationPropertiesManager::clearGarbage(void) {
	for (Property* prop : m_garbage) {
		delete prop;
	}
	m_garbage.clear();

	for (const Property* prop : m_changedProperties) {
		delete prop;
	}
	m_changedProperties.clear();
}

ot::ApplicationPropertiesManager::ApplicationPropertiesManager()
	: m_propertyReplaceOnMerge(false), m_dialog(nullptr)
{}

ot::ApplicationPropertiesManager::~ApplicationPropertiesManager() {
	this->clearGarbage();
	m_data.clear();
}