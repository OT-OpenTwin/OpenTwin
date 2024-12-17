//! @file VersionGraphManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/VersionGraphManager.h"

// Qt header
#include <QtWidgets/qlayout.h>

std::string ot::VersionGraphManager::viewModeToString(ViewMode _mode) {
	switch (_mode) {
	case ot::VersionGraphManager::ViewAll: return "View All";
	case ot::VersionGraphManager::Compact: return "Compact View";
	case ot::VersionGraphManager::CompactLabeled: return "Compact View + Labeled";
	case ot::VersionGraphManager::LabeledOnly: return "Labeled Only";
	default:
		OT_LOG_EAS("Unknown view mode (" + std::to_string((int)_mode) + ")");
		return "View All";
	}
}

ot::VersionGraphManager::ViewMode ot::VersionGraphManager::stringToViewMode(const std::string& _mode) {
	if (_mode == viewModeToString(ViewMode::ViewAll)) return ViewMode::ViewAll;
	else if (_mode == viewModeToString(ViewMode::Compact)) return ViewMode::Compact;
	else if (_mode == viewModeToString(ViewMode::CompactLabeled)) return ViewMode::CompactLabeled;
	else if (_mode == viewModeToString(ViewMode::LabeledOnly)) return ViewMode::LabeledOnly;
	else {
		OT_LOG_EAS("Unknown view mode \"" + _mode + "\"");
		return ViewMode::ViewAll;
	}
}

ot::VersionGraphManager::VersionGraphManager() {
	m_root = new QWidget;
	QVBoxLayout* rootLay = new QVBoxLayout(m_root);
	QHBoxLayout* modeLay = new QHBoxLayout;
	
	m_textFilter = new LineEdit;
	m_textFilter->setPlaceholderText("Find...");
	m_textFilter->setToolTip("Find version containing the text in its label or description (mode independent)");

	Label* modeLabel = new Label("Mode:");
	m_modeSelector = new ComboBox;
	m_modeSelector->addItems({
		QString::fromStdString(viewModeToString(ViewMode::ViewAll)),
		QString::fromStdString(viewModeToString(ViewMode::Compact)),
		QString::fromStdString(viewModeToString(ViewMode::CompactLabeled)),
		QString::fromStdString(viewModeToString(ViewMode::LabeledOnly))
	});
	m_modeSelector->setCurrentText(QString::fromStdString(viewModeToString(ViewMode::ViewAll)));

	m_graph = new VersionGraph;

	rootLay->addLayout(modeLay);
	rootLay->addWidget(m_graph, 1);

	modeLay->setContentsMargins(0, 0, 0, 0);
	modeLay->addWidget(m_textFilter, 1);
	modeLay->addWidget(modeLabel);
	modeLay->addWidget(m_modeSelector);

	this->connect(m_textFilter, &LineEdit::textChanged, this, &VersionGraphManager::updateCurrentGraph);
	this->connect(m_modeSelector, &ComboBox::currentTextChanged, this, &VersionGraphManager::updateCurrentGraph);
}

ot::VersionGraphManager::~VersionGraphManager() {

}

void ot::VersionGraphManager::setupConfig(VersionGraphCfg&& _config) {
	m_config = std::move(_config);
	this->updateCurrentGraph();
}

void ot::VersionGraphManager::setCurrentViewMode(ViewMode _mode) {
	bool blocked = m_modeSelector->signalsBlocked();
	m_modeSelector->blockSignals(true);

	m_modeSelector->setCurrentText(QString::fromStdString(viewModeToString(_mode)));

	m_modeSelector->blockSignals(blocked);

	this->updateCurrentGraph();
}

ot::VersionGraphManager::ViewMode ot::VersionGraphManager::getCurrentViewMode(void) const {
	return stringToViewMode(m_modeSelector->currentText().toStdString());
}

void ot::VersionGraphManager::addVersion(const std::string& _parentVersionName, const VersionGraphVersionCfg& _config) {
	if (_parentVersionName.empty()) {
		m_config.setRootVersion(new VersionGraphVersionCfg(_config));
	}
	else {
		VersionGraphVersionCfg* parent = m_config.findVersion(_parentVersionName);
		if (parent) {
			parent->addChildVersion(new VersionGraphVersionCfg(_config));
		}
		else {
			OT_LOG_EAS("Parent version not found \"" + _parentVersionName + "\"");
		}
	}
	
}

ot::VersionGraphVersionCfg* ot::VersionGraphManager::addVersion(const ConstJsonObject& _versionConfig) {
	VersionGraphVersionCfg* newVersion = new VersionGraphVersionCfg;
	if (!newVersion->setFromJsonObject(_versionConfig, &m_config)) {
		delete newVersion;
		newVersion = nullptr;
	}
	return newVersion;
}

void ot::VersionGraphManager::activateVersion(const std::string& _versionName, const std::string& _activeBranchVersionName) {
	m_config.setActiveVersionName(_versionName);
	m_config.setActiveBranchVersionName(_activeBranchVersionName);
	this->updateCurrentGraph();
}

void ot::VersionGraphManager::removeVersion(const std::string& _versionName) {
	m_config.removeVersion(_versionName);
	this->updateCurrentGraph();
}

void ot::VersionGraphManager::removeVersions(const std::list<std::string>& _versionNames) {
	for (const std::string& version : _versionNames) {
		m_config.removeVersion(version);
	}
	this->updateCurrentGraph();
}

void ot::VersionGraphManager::updateCurrentGraph(void) {	
	switch (this->getCurrentViewMode()) {
	case ot::VersionGraphManager::ViewAll:
		this->updateCurrentGraphViewAllMode();
		break;

	case ot::VersionGraphManager::Compact:
		this->updateCurrentGraphCompactMode();
		break;

	case ot::VersionGraphManager::CompactLabeled:
		this->updateCurrentGraphCompactLabelMode();
		break;

	case ot::VersionGraphManager::LabeledOnly:
		this->updateCurrentGraphLabeledOnlyMode();
		break;

	default:
		OT_LOG_EAS("Unknown view mode (" + std::to_string((int)this->getCurrentViewMode()) + ")");
		m_graph->setupFromConfig(m_config);
		break;
	}
}

void ot::VersionGraphManager::updateCurrentGraphViewAllMode(void) {
	VersionGraphCfg newConfig;
	newConfig.setActiveVersionName(m_config.getActiveVersionName());

	// Determine branch version
	if (!m_config.getActiveBranchVersionName().empty()) {
		VersionGraphVersionCfg* branchVersion = m_config.findVersion(m_config.getActiveBranchVersionName());
		if (branchVersion) {
			newConfig.setActiveBranchVersionName(branchVersion->getLastBranchVersion()->getName());
		}
		else {
			OT_LOG_EAS("Branch version not found: \"" + m_config.getActiveBranchVersionName() + "\"");
		}
	}

	// Copy root config
	VersionGraphVersionCfg* newItem = new VersionGraphVersionCfg;
	newItem->applyConfigOnly(*m_config.getRootVersion());

	// Process item childs
	for (const VersionGraphVersionCfg* childCfg : m_config.getRootVersion()->getChildVersions()) {
		this->processViewAllWithTextFilter(newItem, childCfg, newConfig.getActiveVersionName(), true, m_textFilter->text());
	}

	newConfig.setRootVersion(newItem);
	m_graph->setupFromConfig(newConfig);
}

void ot::VersionGraphManager::updateCurrentGraphCompactMode(void) {
	this->startProcessCompact(false, m_textFilter->text());
}

void ot::VersionGraphManager::updateCurrentGraphCompactLabelMode(void) {
	this->startProcessCompact(true, m_textFilter->text());
}

void ot::VersionGraphManager::updateCurrentGraphLabeledOnlyMode(void) {
	VersionGraphCfg newConfig;
	newConfig.setActiveVersionName(m_config.getActiveVersionName());

	// Determine branch version
	if (!m_config.getActiveBranchVersionName().empty()) {
		VersionGraphVersionCfg* branchVersion = m_config.findVersion(m_config.getActiveBranchVersionName());
		if (branchVersion) {
			newConfig.setActiveBranchVersionName(branchVersion->getLastBranchVersion()->getName());
		}
		else {
			OT_LOG_EAS("Branch version not found: \"" + m_config.getActiveBranchVersionName() + "\"");
		}
	}

	// Copy root config
	VersionGraphVersionCfg* newItem = new VersionGraphVersionCfg;
	newItem->applyConfigOnly(*m_config.getRootVersion());

	// Process item childs
	for (const VersionGraphVersionCfg* childCfg : m_config.getRootVersion()->getChildVersions()) {
		this->processLabeledOnlyItem(newItem, childCfg, newConfig.getActiveVersionName(), true, m_textFilter->text());
	}

	newConfig.setRootVersion(newItem);
	m_graph->setupFromConfig(newConfig);
}

bool ot::VersionGraphManager::checkFilterValid(const VersionGraphVersionCfg* _versionConfig, const QString& _filterText) const {
	if (_filterText.isEmpty()) {
		return true;
	}
	else if (QString::fromStdString(_versionConfig->getLabel()).contains(_filterText, Qt::CaseInsensitive) || 
		QString::fromStdString(_versionConfig->getDescription()).contains(_filterText, Qt::CaseInsensitive)) 
	{
		return true;
	}
	else {
		return false;
	}
}

void ot::VersionGraphManager::processViewAllWithTextFilter(VersionGraphVersionCfg* _parent, const VersionGraphVersionCfg* _config, const std::string& _activeVersion, bool _isDirectParent, const QString& _filterText) {
	VersionGraphVersionCfg* newParent = _parent;
	bool newParentIsDirect = false;

	// Active item
	if (_config->getName() == _activeVersion || this->checkFilterValid(_config, _filterText)) {
		VersionGraphVersionCfg* newItem = new VersionGraphVersionCfg;
		newItem->applyConfigOnly(*_config);
		newItem->setDirectParentIsHidden(!_isDirectParent);

		_parent->addChildVersion(newItem);
		newParent = newItem;
		newParentIsDirect = true;
	}

	// Process childs
	for (const VersionGraphVersionCfg* childCfg : _config->getChildVersions()) {
		this->processViewAllWithTextFilter(newParent, childCfg, _activeVersion, newParentIsDirect, _filterText);
	}
}

void ot::VersionGraphManager::startProcessCompact(bool _includeLabeledVersions, const QString& _filterText) {
	VersionGraphCfg newConfig;
	newConfig.setActiveVersionName(m_config.getActiveVersionName());

	// Determine branch version
	if (!m_config.getActiveBranchVersionName().empty()) {
		VersionGraphVersionCfg* branchVersion = m_config.findVersion(m_config.getActiveBranchVersionName());
		if (branchVersion) {
			newConfig.setActiveBranchVersionName(branchVersion->getLastBranchVersion()->getName());
		}
		else {
			OT_LOG_EAS("Branch version not found: \"" + m_config.getActiveBranchVersionName() + "\"");
		}
	}

	// Copy root config
	VersionGraphVersionCfg* newItem = new VersionGraphVersionCfg;
	newItem->applyConfigOnly(*m_config.getRootVersion());

	// Process item childs
	for (const VersionGraphVersionCfg* childCfg : m_config.getRootVersion()->getChildVersions()) {
		this->processCompactItem(newItem, childCfg, newConfig.getActiveVersionName(), true, _includeLabeledVersions, _filterText);
	}

	newConfig.setRootVersion(newItem);
	m_graph->setupFromConfig(newConfig);
}

void ot::VersionGraphManager::processCompactItem(VersionGraphVersionCfg* _parent, const VersionGraphVersionCfg* _config, const std::string& _activeVersion, bool _isDirectParent, bool _includeLabeledVersions, const QString& _filterText) {
	VersionGraphVersionCfg* newItem = new VersionGraphVersionCfg;
	newItem->applyConfigOnly(*_config);
	newItem->setDirectParentIsHidden(!_isDirectParent);

	// Active item or filter match
	if (_config->getName() == _activeVersion) {
		// All direct childs of an active item must be visible
		for (const VersionGraphVersionCfg* childCfg : _config->getChildVersions()) {
			VersionGraphVersionCfg* childItem = new VersionGraphVersionCfg;
			childItem->applyConfigOnly(*childCfg);
			childItem->setDirectParentIsHidden(false);

			for (const VersionGraphVersionCfg* childsChildCfg : childCfg->getChildVersions()) {
				this->processCompactItem(childItem, childsChildCfg, _activeVersion, true, _includeLabeledVersions, _filterText);
			}

			newItem->addChildVersion(childItem);
		}

		_parent->addChildVersion(newItem);
	}

	// Middle item, not active
	else if (_config->getChildVersions().size() == 1) {
		const VersionGraphVersionCfg* childCfg = _config->getChildVersions().front();

		// Child is active
		if ((childCfg->getName() == _activeVersion || (_includeLabeledVersions && !newItem->getLabel().empty())) && 
			this->checkFilterValid(_config, _filterText)) // if a filter is set this needs to match the filter
		{
			this->processCompactItem(newItem, childCfg, _activeVersion, true, _includeLabeledVersions, _filterText);
			_parent->addChildVersion(newItem);
		}
		// Process other childs
		else {
			delete newItem;
			newItem = nullptr;
			this->processCompactItem(_parent, childCfg, _activeVersion, false, _includeLabeledVersions, _filterText);
		}
		
	}
	
	// Last item or branch item with matching filter (item is shown)
	else if (this->checkFilterValid(_config, _filterText) || _config->getChildVersions().size() == 0) {
		// Process item childs
		for (const VersionGraphVersionCfg* childCfg : _config->getChildVersions()) {
			this->processCompactItem(newItem, childCfg, _activeVersion, true, _includeLabeledVersions, _filterText);
		}

		_parent->addChildVersion(newItem);
	}

	// Last item or branch item without filter match (item is hidden)
	else {
		delete newItem;
		newItem = nullptr;

		// Process item childs
		for (const VersionGraphVersionCfg* childCfg : _config->getChildVersions()) {
			this->processCompactItem(_parent, childCfg, _activeVersion, false, _includeLabeledVersions, _filterText);
		}
	}
}

void ot::VersionGraphManager::processLabeledOnlyItem(VersionGraphVersionCfg* _parent, const VersionGraphVersionCfg* _config, const std::string& _activeVersion, bool _isDirectParent, const QString& _filterText) {
	VersionGraphVersionCfg* newParent = _parent;
	bool newParentIsDirect = false;

	// Active item or labeled item (with filter match on labeled)
	if (_config->getName() == _activeVersion || 
		(!_config->getLabel().empty() && this->checkFilterValid(_config, _filterText)))
	{
		VersionGraphVersionCfg* newItem = new VersionGraphVersionCfg;
		newItem->applyConfigOnly(*_config);
		newItem->setDirectParentIsHidden(!_isDirectParent);

		_parent->addChildVersion(newItem);
		newParent = newItem;
		newParentIsDirect = true;
	}

	// Process childs
	for (const VersionGraphVersionCfg* childCfg : _config->getChildVersions()) {
		this->processLabeledOnlyItem(newParent, childCfg, _activeVersion, newParentIsDirect, _filterText);
	}
}
