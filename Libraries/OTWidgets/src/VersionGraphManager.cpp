//! @file VersionGraphManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
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

void ot::VersionGraphManager::addVersion(VersionGraphVersionCfg&& _config) {
	m_config.addVersion(std::move(_config));
}

ot::VersionGraphVersionCfg* ot::VersionGraphManager::addVersion(const ConstJsonObject& _versionConfig) {
	VersionGraphVersionCfg* newVersion = new VersionGraphVersionCfg;
	newVersion->setFromJsonObject(_versionConfig);
	return newVersion;
}

void ot::VersionGraphManager::activateVersion(const std::string& _versionName, const std::string& _activeBranchVersionName) {
	m_config.setActiveVersionName(_versionName);
	m_config.setActiveBranchName(_activeBranchVersionName);
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
	newConfig.setActiveBranchName(m_config.getActiveBranchName());

	std::list<std::list<VersionGraphVersionCfg>> newBranches;
	ParentInfo parentInfo;
	std::string filterText = m_textFilter->text().toStdString();

	// Go trough all branches
	for (const std::list<VersionGraphVersionCfg>& branchVersions : m_config.getBranches()) {
		std::list<VersionGraphVersionCfg> newVersions;

		// Go trough all versions
		bool isFirst = true;
		for (const VersionGraphVersionCfg& version : branchVersions) {
			// Check if already a parent exists, so we know we are in a branch
			if (parentInfo.parent && isFirst) {
				// Update parent info to branch node
				parentInfo.parent = nullptr;
				
				if (!this->findParentVersionInOtherBranches(version, newBranches, parentInfo.parent)) {
					OT_LOG_EA("Failed to find parent branch");
				}
				
			}
			isFirst = false;

			// Check if the version should be added
			if (this->filterModeAll(version, newConfig.getActiveVersionName(), filterText, parentInfo)) {
				VersionGraphVersionCfg newVersion(version);

				// Set parent info
				if (parentInfo.parent) {
					newVersion.setParentVersion(parentInfo.parent->getName());
				}
				else {
					newVersion.setParentVersion("");
				}
				newVersion.setDirectParentIsHidden(!(parentInfo.flags & ParentFlag::IsDirect));
				newVersions.push_back(std::move(newVersion));

				// Update current parent info
				parentInfo.parent = &version;
				parentInfo.flags |= ParentFlag::IsDirect;
				if (version == newConfig.getActiveVersionName()) {
					parentInfo.flags |= ParentFlag::IsActive;
				}
				else {
					parentInfo.flags &= ~ParentFlag::IsActive;
				}
			}
			else {
				// Version should not be added, only update current parent info
				parentInfo.flags &= ~ParentFlag::IsDirect;
			}
		}

		if (!newVersions.empty()) {
			newBranches.push_back(std::move(newVersions));
		}
	}

	// Apply new config
	newConfig.setBranches(std::move(newBranches));
	m_graph->setupFromConfig(newConfig);
}

void ot::VersionGraphManager::updateCurrentGraphCompactMode(void) {
	//this->startProcessCompact(false, m_textFilter->text());
}

void ot::VersionGraphManager::updateCurrentGraphCompactLabelMode(void) {
	//this->startProcessCompact(true, m_textFilter->text());
}

void ot::VersionGraphManager::updateCurrentGraphLabeledOnlyMode(void) {
/*
	VersionGraphCfg newConfig;
	newConfig.setActiveVersionName(m_config.getActiveVersionName());

	// Determine branch version
	if (!m_config.getActiveBranchName().empty()) {
		VersionGraphVersionCfg* branchVersion = m_config.findVersion(m_config.getActiveBranchName());
		if (branchVersion) {
			newConfig.setActiveBranchVersionName(branchVersion->getLastBranchVersion()->getName());
		}
		else {
			OT_LOG_EAS("Branch version not found: \"" + m_config.getActiveBranchName() + "\"");
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
	*/
}

bool ot::VersionGraphManager::isVersionGreater(const std::string& _left, const std::string& _right) const {
	size_t ixLeft = _left.rfind('.');
	size_t ixRight = _left.rfind('.');

	int versionLeft = -1;
	int versionRight = -1;

	if (ixLeft != std::string::npos) {
		if (ixRight == std::string::npos) {
			OT_LOG_E("Comparing different branches");
			return false;
		}

		bool failed = false;
		versionLeft = String::toNumber<int>(_left.substr(ixLeft + 1), failed);
		if (failed) {
			OT_LOG_E("Invalid number format: \"" + _left.substr(ixLeft) + "\"");
			return false;
		}
		versionRight = String::toNumber<int>(_right.substr(ixRight + 1), failed);
		if (failed) {
			OT_LOG_E("Invalid number format: \"" + _right.substr(ixRight) + "\"");
			return false;
		}
	}
	else if (ixRight == std::string::npos) {
		bool failed = false;

		versionLeft = String::toNumber<int>(_left, failed);
		if (failed) {
			OT_LOG_E("Invalid number format: \"" + _left + "\"");
			return false;
		}
		
		versionRight = String::toNumber<int>(_right, failed);
		if (failed) {
			OT_LOG_E("Invalid number format: \"" + _right + "\"");
			return false;
		}
	}
	else {
		OT_LOG_E("Comparing different branches { \"Left\": \"" + _left + "\", \"Right\": \"" + _right + "\" }");
		return false;
	}

	return versionLeft > versionRight;
}

bool ot::VersionGraphManager::findParentVersionInOtherBranches(const VersionGraphVersionCfg& _version, const std::list<std::list<VersionGraphVersionCfg>>& _branches, const VersionGraphVersionCfg*& _parent) {
	VersionGraphVersionCfg tmpVersion(_version);

	// Check in current branch
	tmpVersion.setName(tmpVersion.getBranchNodeName()); // Next parent branch
	while (!tmpVersion.getName().empty()) {
		for (const std::list<VersionGraphVersionCfg>& otherBranchVersions : _branches) {
			OTAssert(!otherBranchVersions.empty(), "Empty branch stored");

			if (otherBranchVersions.front().getBranchName() == tmpVersion.getBranchName()) {
				const VersionGraphVersionCfg* lastVersion = nullptr;

				for (const VersionGraphVersionCfg& otherVersion : otherBranchVersions) {
					if (otherVersion.getName() == tmpVersion.getName()) {
						// Direct parent found
						_parent = &otherVersion;
						return true;
					}
					else if (!this->isVersionGreater(otherVersion.getName(), tmpVersion.getName())) {
						OTAssert((lastVersion ? this->isVersionGreater(otherVersion.getName(), lastVersion->getName()) : true), "Invalid version order");

						// Version is smaller than the direct parent, remember last
						lastVersion = &otherVersion;
					}
				}

				// Direct parent version not found but one of its parents.
				if (lastVersion) {
					_parent = lastVersion;
					return true;
				}
			}
		}

		tmpVersion.setName(tmpVersion.getBranchNodeName()); // Next parent branch
	}

	return false;
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

bool ot::VersionGraphManager::filterModeAll(const VersionGraphVersionCfg& _thisVersion, const std::string& _activeVersionName, const std::string& _filterText, const ParentInfo& _parentInfo) {
	if (_thisVersion.getName() == _activeVersionName || _filterText.empty() || _thisVersion.getName() == "1") {
		return true;
	}
	QString filterText = QString::fromStdString(_filterText).toLower();
	QString entry = QString::fromStdString(_thisVersion.getName()).toLower();
	if (entry.contains(filterText, Qt::CaseInsensitive)) {
		return true;
	}
	
	entry = QString::fromStdString(_thisVersion.getLabel()).toLower();
	if (entry.contains(filterText, Qt::CaseInsensitive)) {
		return true;
	}

	entry = QString::fromStdString(_thisVersion.getDescription()).toLower();
	if (entry.contains(filterText, Qt::CaseInsensitive)) {
		return true;
	}

	return false;
}

/*
void ot::VersionGraphManager::startProcessCompact(bool _includeLabeledVersions, const QString& _filterText) {
	VersionGraphCfg newConfig;
	newConfig.setActiveVersionName(m_config.getActiveVersionName());

	// Determine branch version
	if (!m_config.getActiveBranchName().empty()) {
		VersionGraphVersionCfg* branchVersion = m_config.findVersion(m_config.getActiveBranchName());
		if (branchVersion) {
			newConfig.setActiveBranchVersionName(branchVersion->getLastBranchVersion()->getName());
		}
		else {
			OT_LOG_EAS("Branch version not found: \"" + m_config.getActiveBranchName() + "\"");
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
*/

ot::VersionGraphManager::ParentInfo::ParentInfo() 
	: parent(nullptr), flags(ParentFlag::None)
{}
