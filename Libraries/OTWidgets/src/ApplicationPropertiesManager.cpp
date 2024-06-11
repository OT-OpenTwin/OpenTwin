//! @file ApplicationPropertiesManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/ApplicationPropertiesManager.h"

ot::ApplicationPropertiesManager& ot::ApplicationPropertiesManager::instance(void) {
	static ApplicationPropertiesManager g_instance;
	return g_instance;
}

ot::Dialog::DialogResult ot::ApplicationPropertiesManager::showDialog(void) {
	// Build configuration
	PropertyGridCfg gridCfg;

	for (const auto& it : m_data) {
		PropertyGroup* newGroup = new PropertyGroup(it.first);
		newGroup->mergeWith(*it.second.getDefaultGroup(), m_propertyReplaceOnMerge);
		for (PropertyGroup* newChild : it.second.getRootGroups()) {
			if (!newChild->isEmpty()) {
				newGroup->addChildGroup(new PropertyGroup(*newChild));
			}
		}

		if (newGroup) {
			gridCfg.addRootGroup(newGroup);
		}
		else {
			// Skip empty groups
			delete newGroup;
		}
	}

	// If there are no properties skip showing the dialog.
	if (gridCfg.isEmpty()) {
		return ot::Dialog::Cancel;
	}

	// Show dialog
	PropertyDialogCfg dialogCfg;
	dialogCfg.setGridConfig(gridCfg);	

	OTAssert(m_dialog == nullptr, "Dialog already exists");
	m_dialog = new PropertyDialog(dialogCfg);

	this->connect(m_dialog, &PropertyDialog::propertyChanged, this, &ApplicationPropertiesManager::slotPropertyChanged);
	this->connect(m_dialog, &PropertyDialog::propertyDeleteRequested, this, &ApplicationPropertiesManager::slotPropertyDeleteRequested);

	Dialog::DialogResult result = m_dialog->showDialog();

	this->disconnect(m_dialog, &PropertyDialog::propertyChanged, this, &ApplicationPropertiesManager::slotPropertyChanged);
	this->disconnect(m_dialog, &PropertyDialog::propertyDeleteRequested, this, &ApplicationPropertiesManager::slotPropertyDeleteRequested);

	delete m_dialog;
	m_dialog = nullptr;

	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ApplicationPropertiesManager::add(const std::string& _owner, const PropertyGridCfg& _config) {
	this->findOrCreateData(_owner).mergeWith(_config, m_propertyReplaceOnMerge);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ot::ApplicationPropertiesManager::slotPropertyChanged(const std::string& _groupPath, const std::string& _propertyName) {
	Q_EMIT propertyChanged(_groupPath, _propertyName);
}

void ot::ApplicationPropertiesManager::slotPropertyDeleteRequested(const std::string& _groupPath, const std::string& _propertyName) {
	Q_EMIT propertyDeleteRequested(_groupPath, _propertyName);
}

ot::PropertyGridCfg& ot::ApplicationPropertiesManager::findOrCreateData(const std::string& _owner) {
	auto it = m_data.find(_owner);
	if (it == m_data.end()) {
		m_data.insert_or_assign(_owner, PropertyGridCfg());
		it = m_data.find(_owner);
	}
	OTAssert(it != m_data.end(), "Entry not found");
	return it->second;
}

ot::ApplicationPropertiesManager::ApplicationPropertiesManager()
	: m_propertyReplaceOnMerge(false), m_dialog(nullptr)
{}

ot::ApplicationPropertiesManager::~ApplicationPropertiesManager() {

}