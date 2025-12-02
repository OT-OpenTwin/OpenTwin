// @otlicense
// File: ApplicationPropertiesManager.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/ApplicationPropertiesManager.h"

ot::ApplicationPropertiesManager& ot::ApplicationPropertiesManager::instance(void) {
	static ApplicationPropertiesManager g_instance;
	return g_instance;
}

ot::Dialog::DialogResult ot::ApplicationPropertiesManager::showDialog(void) {
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
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ApplicationPropertiesManager::add(const std::string& _owner, const PropertyGridCfg& _config) {
	PropertyGridCfg oldConfig = this->findData(_owner);
	oldConfig.mergeWith(_config, (m_propertyReplaceOnMerge ? PropertyBase::FullMerge : PropertyBase::AddMissing | PropertyBase::MergeValues));
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
	std::unique_ptr<Property> cleanedProperty(this->createCleanedSlotProperty(_property, owner));
	OTAssertNullptr(cleanedProperty);
	Q_EMIT propertyChanged(owner, cleanedProperty.get());
}

void ot::ApplicationPropertiesManager::slotPropertyDeleteRequested(const Property* _property) {
	std::string owner;
	std::unique_ptr<Property> cleanedProperty(this->createCleanedSlotProperty(_property, owner));
	OTAssertNullptr(cleanedProperty);
	Q_EMIT propertyDeleteRequested(owner, cleanedProperty.get());
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
		for (const PropertyGroup* newChild : it.second.getRootGroups()) {
			if (!newChild->isEmpty()) {
				newGroup->addChildGroup(new PropertyGroup(newChild));
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

ot::ApplicationPropertiesManager::ApplicationPropertiesManager() :
	m_propertyReplaceOnMerge(false), m_dialog(nullptr)
{}

ot::ApplicationPropertiesManager::~ApplicationPropertiesManager() {
	m_data.clear();
	if (m_dialog) {
		delete m_dialog;
		m_dialog = nullptr;
	}
}