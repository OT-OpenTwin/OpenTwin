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

	Dialog::DialogResult result = Dialog::Cancel;

	{
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

		result = m_dialog->showDialog();

		this->disconnect(m_dialog, &PropertyDialog::propertyChanged, this, &ApplicationPropertiesManager::slotPropertyChanged);
		this->disconnect(m_dialog, &PropertyDialog::propertyDeleteRequested, this, &ApplicationPropertiesManager::slotPropertyDeleteRequested);

		delete m_dialog;
		m_dialog = nullptr;
	}
	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ApplicationPropertiesManager::add(const std::string& _owner, const PropertyGridCfg& _config) {
	PropertyGridCfg oldConfig = this->findData(_owner);
	oldConfig.mergeWith(_config, m_propertyReplaceOnMerge);
	m_data.insert_or_assign(_owner, oldConfig);
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
	ot::Property* newProperty = _property->createCopy();
	OTAssertNullptr(newProperty);

	std::list<const ot::PropertyGroup*> parents = this->getParentGroups(_property);
	if (parents.size() < 2) {
		OT_LOG_EA("Data mismatch");
		m_garbage.push_back(newProperty);
		return newProperty;
	}

	_owner = parents.front()->getName();
	parents.pop_front();
	
	ot::PropertyGroup* newGroup = parents.front()->createCopy(false);
	parents.pop_front();
	while (!parents.empty()) {
		ot::PropertyGroup* tmp = parents.front()->createCopy(false);
		newGroup->addChildGroup(tmp);
		tmp = newGroup;
	}

	newGroup->addProperty(newProperty);

	//m_garbage.push_back(newProperty);

	return newProperty;
}

std::list<const ot::PropertyGroup*> ot::ApplicationPropertiesManager::getParentGroups(const Property* _property) const {
	std::list<const ot::PropertyGroup*> result;
	ot::PropertyGroup* group = _property->getParentGroup();
	while (group) {
		result.push_front(group);
		group = group->getParentGroup();
	}
	return result;
}

void ot::ApplicationPropertiesManager::clearGarbage(void) {
	for (Property* prop : m_garbage) {
		delete prop;
	}
	m_garbage.clear();
}

ot::ApplicationPropertiesManager::ApplicationPropertiesManager()
	: m_propertyReplaceOnMerge(false), m_dialog(nullptr)
{}

ot::ApplicationPropertiesManager::~ApplicationPropertiesManager() {
	this->clearGarbage();
	m_data.clear();
}