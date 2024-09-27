//! @file VersionGraphManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/VersionGraphManager.h"

// Qt header
#include <QtWidgets/qlayout.h>

#define OT_VERSIONGRAPHMANAGER_AllMode "View All"
#define OT_VERSIONGRAPHMANAGER_CompactMode "Compact View"
#define OT_VERSIONGRAPHMANAGER_CompactWithLabelMode "Compact View + Labeled"

ot::VersionGraphManager::VersionGraphManager() {
	m_root = new QWidget;
	QVBoxLayout* rootLay = new QVBoxLayout(m_root);
	QHBoxLayout* modeLay = new QHBoxLayout;
	
	Label* modeLabel = new Label("Mode:");
	m_modeSelector = new ComboBox;
	m_modeSelector->addItems({
		OT_VERSIONGRAPHMANAGER_AllMode,
		OT_VERSIONGRAPHMANAGER_CompactMode,
		OT_VERSIONGRAPHMANAGER_CompactWithLabelMode
	});
	m_modeSelector->setCurrentText(OT_VERSIONGRAPHMANAGER_AllMode);

	m_graph = new VersionGraph;

	rootLay->addLayout(modeLay);
	rootLay->addWidget(m_graph, 1);

	modeLay->setContentsMargins(0, 0, 0, 0);
	modeLay->addStretch(1);
	modeLay->addWidget(modeLabel);
	modeLay->addWidget(m_modeSelector);

	this->connect(m_modeSelector, &ComboBox::currentTextChanged, this, &VersionGraphManager::updateCurrentGraph);
}

ot::VersionGraphManager::~VersionGraphManager() {

}

void ot::VersionGraphManager::setupConfig(const VersionGraphCfg& _config) {
	m_config = _config;
	this->updateCurrentGraph();
}

void ot::VersionGraphManager::activateVersion(const std::string& _versionName) {
	m_config.setActiveVersionName(_versionName);
	this->updateCurrentGraph();
}

void ot::VersionGraphManager::updateCurrentGraph(void) {
	m_graph->clear();

	if (m_modeSelector->currentText() == OT_VERSIONGRAPHMANAGER_AllMode) m_graph->setupFromConfig(m_config);
	else if (m_modeSelector->currentText() == OT_VERSIONGRAPHMANAGER_CompactMode) this->updateCurrentGraphCompactMode();
	else if (m_modeSelector->currentText() == OT_VERSIONGRAPHMANAGER_CompactWithLabelMode) this->updateCurrentGraphCompactLabelMode();
	else {
		OT_LOG_EAS("Invalid mode selected \"" + m_modeSelector->currentText().toStdString() + "\"");
		m_graph->setupFromConfig(m_config);
	}
}

void ot::VersionGraphManager::updateCurrentGraphCompactMode(void) {
	this->startProcessCompact(false);
}

void ot::VersionGraphManager::updateCurrentGraphCompactLabelMode(void) {
	this->startProcessCompact(true);
}

void ot::VersionGraphManager::startProcessCompact(bool _includeLabeledVersions) {
	VersionGraphCfg newConfig;
	newConfig.setActiveVersionName(m_config.getActiveVersionName());

	// Determine branch version
	VersionGraphVersionCfg* branchVersion = m_config.getRootVersion()->findVersion(m_config.getActiveBranchVersionName());
	if (branchVersion) {
		newConfig.setActiveBranchVersionName(branchVersion->getLastBranchVersion()->getName());
	}
	else {
		OT_LOG_E("Branch version not found");
	}

	// Copy root config
	VersionGraphVersionCfg* newItem = new VersionGraphVersionCfg;
	newItem->applyConfigOnly(*m_config.getRootVersion());

	// Process item childs
	for (const VersionGraphVersionCfg* childCfg : m_config.getRootVersion()->getChildVersions()) {
		this->processCompactItem(newItem, childCfg, newConfig.getActiveVersionName(), true, _includeLabeledVersions);
	}

	newConfig.setRootVersion(newItem);
	m_graph->setupFromConfig(newConfig);
}

void ot::VersionGraphManager::processCompactItem(VersionGraphVersionCfg* _parent, const VersionGraphVersionCfg* _config, const std::string& _activeVersion, bool _isDirectParent, bool _includeLabeledVersions) {
	VersionGraphVersionCfg* newItem = new VersionGraphVersionCfg;
	newItem->applyConfigOnly(*_config);
	newItem->setDirectParentIsHidden(!_isDirectParent);

	// Active item
	if (_config->getName() == _activeVersion) {
		// All direct childs of an active item must be visible
		for (const VersionGraphVersionCfg* childCfg : _config->getChildVersions()) {
			VersionGraphVersionCfg* childItem = new VersionGraphVersionCfg;
			childItem->applyConfigOnly(*childCfg);
			childItem->setDirectParentIsHidden(false);

			for (const VersionGraphVersionCfg* childsChildCfg : childCfg->getChildVersions()) {
				this->processCompactItem(childItem, childsChildCfg, _activeVersion, true, _includeLabeledVersions);
			}

			newItem->addChildVersion(childItem);
		}

		_parent->addChildVersion(newItem);
	}
	// Middle item, not active
	else if (_config->getChildVersions().size() == 1) {
		const VersionGraphVersionCfg* childCfg = _config->getChildVersions().front();

		// Child is active
		if (childCfg->getName() == _activeVersion || (_includeLabeledVersions && !newItem->getLabel().empty())) {
			this->processCompactItem(newItem, childCfg, _activeVersion, true, _includeLabeledVersions);
			_parent->addChildVersion(newItem);
		}
		// Process other childs
		else {
			this->processCompactItem(_parent, childCfg, _activeVersion, false, _includeLabeledVersions);
		}
		
	}
	// Last item or branch item
	else {
		// Process item childs
		for (const VersionGraphVersionCfg* childCfg : _config->getChildVersions()) {
			this->processCompactItem(newItem, childCfg, _activeVersion, true, _includeLabeledVersions);
		}

		_parent->addChildVersion(newItem);
	}
}
