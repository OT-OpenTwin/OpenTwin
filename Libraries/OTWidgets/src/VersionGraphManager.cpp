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

namespace ot {

	class VersionGraphManager::VersionInfo {
		OT_DECL_NOCOPY(VersionInfo)
	public:
		enum VersionInfoFlag {
			None           = 0 << 0, //! \brief No information.
			IsActive       = 1 << 1, //! \brief Version is active version.

			//! \brief Version has branches.
			//! For example only version 2 would need this flag set: <br>
			//!  <br>
			//! 1 ----- 2 --+-- 3 ----- ...     <br>
			//!             |                   <br>
			//!             +-- 2.1.1 ----- ... <br>
			//!             |                   <br>
			//!             +-- 2.2.1 ----- ... <br>
			//! 
			IsBranchNode   = 1 << 2,
			IsEndOfBranch  = 1 << 3, //! \brief Version is end of branch
			IsDirect       = 1 << 4,  //! \brief Version is direct parent/child
			IsOnActivePath = 1 << 5
		};
		OT_ADD_FRIEND_FLAG_FUNCTIONS(VersionInfoFlag)
		typedef Flags<VersionInfoFlag> VersionInfoFlags;

		VersionInfo() : m_flags(VersionInfoFlag::None), m_versionConfig(nullptr) {};
		VersionInfo(VersionInfo&& _other) noexcept = delete;
		VersionInfo& operator = (VersionInfo&& _other) noexcept {
			if (this != &_other) {
				m_flags = std::move(_other.m_flags);
				m_versionConfig = _other.m_versionConfig;
				_other.m_versionConfig = nullptr;
			}
			return *this;
		}

		void setFlag(VersionInfoFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setFlags(const VersionInfoFlags& _flags) { m_flags = _flags; };
		const VersionInfoFlags& getFlags(void) const { return m_flags; };

		void setConfig(const VersionGraphVersionCfg* _config) { m_versionConfig = _config; };
		const VersionGraphVersionCfg* getConfig(void) const { return m_versionConfig; };

		void applyActiveAndBranchFlags(const VersionGraphCfg& _graphConfig) {
			if (!m_versionConfig) {
				return;
			}

			m_flags.setFlag(IsActive, m_versionConfig->getName() == _graphConfig.getActiveVersionName());
			m_flags.setFlag(IsBranchNode, _graphConfig.getBranchesCountFromNode(m_versionConfig->getName()) > 0);
			m_flags.setFlag(IsOnActivePath, m_versionConfig->isOnActivePath(_graphConfig.getActiveBranchName()));
		}

	private:
		VersionInfoFlags m_flags;
		const VersionGraphVersionCfg* m_versionConfig;
	};

}

std::string ot::VersionGraphManager::viewModeToString(ViewMode _mode) {
	switch (_mode) {
	case ot::VersionGraphManager::ViewAll: return "View All";
	case ot::VersionGraphManager::Compact: return "Compact View";
	case ot::VersionGraphManager::CompactLabeled: return "Compact View + Labeled";
	case ot::VersionGraphManager::LabeledOnly: return "Labeled Only";
	case ot::VersionGraphManager::Iterator: return "Iterator";
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
	else if (_mode == viewModeToString(ViewMode::Iterator)) return ViewMode::Iterator;
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
		QString::fromStdString(viewModeToString(ViewMode::LabeledOnly)),
		QString::fromStdString(viewModeToString(ViewMode::Iterator))
	});
	m_modeSelector->setCurrentText(QString::fromStdString(viewModeToString(ViewMode::Compact)));

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

ot::VersionGraphVersionCfg& ot::VersionGraphManager::insertVersion(VersionGraphVersionCfg&& _config) {
	return m_config.insertVersion(std::move(_config));
}

ot::VersionGraphVersionCfg* ot::VersionGraphManager::insertVersion(const ConstJsonObject& _versionConfig) {
	VersionGraphVersionCfg newVersion;
	newVersion.setFromJsonObject(_versionConfig);
	if (newVersion.getName().empty()) {
		OT_LOG_EA("Invalid version configuration");
		return nullptr;
	}
	else {
		return &this->insertVersion(std::move(newVersion));
	}
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

bool ot::VersionGraphManager::versionIsBranchNode(const std::string& _versionName) {
	OTAssert(!_versionName.empty(), "Empty version name");
	for (const VersionGraphCfg::VersionsList& branchVersions : m_config.getBranches()) {
		OTAssert(!branchVersions.empty(), "Empty branch stored");
		if (branchVersions.front().getBranchNodeName() == _versionName) {
			return true;
		}
	}
	return false;
}

void ot::VersionGraphManager::updateCurrentGraph(void) {
	ViewMode viewMode = this->getCurrentViewMode();
	
	VersionGraphCfg newConfig;
	newConfig.setActiveVersionName(m_config.getActiveVersionName());
	newConfig.setActiveBranchName(m_config.getActiveBranchName());

	VersionInfo parentInfo;
	std::string filterText = m_textFilter->text().toStdString();

	// Go trough all branches
	for (const std::list<VersionGraphVersionCfg>& branchVersions : m_config.getBranches()) {
		std::list<VersionGraphVersionCfg> newVersions;

		// Go trough all versions
		bool isFirst = true;
		for (auto versionIt = branchVersions.begin(); versionIt != branchVersions.end(); versionIt++) {
			// Check if already a parent exists, so we know we are in a branch
			if (!versionIt->isValid()) {
				OT_LOG_E("Invalid version stored. Version: \"" + versionIt->getName() + "\"");
				continue;
			}

			// Set current info
			VersionInfo currentInfo;
			currentInfo.setConfig(&(*versionIt));
			currentInfo.applyActiveAndBranchFlags(m_config);
			currentInfo.setFlag(VersionInfo::IsEndOfBranch, &(*versionIt) == &branchVersions.back());

			// Check if we are at the beginning of a branch and are not on the main branch.
			if (parentInfo.getConfig() && isFirst) {
				// Update parent info to branch node
				parentInfo.setConfig(nullptr);

				if (!this->findParentVersionInOtherBranches(currentInfo.getConfig()->getName(), newConfig.getBranches(), parentInfo)) {
					OT_LOG_EAS("Failed to find parent branch of version \"" + currentInfo.getConfig()->getName() + "\"");
				}
				parentInfo.applyActiveAndBranchFlags(m_config);

				if (parentInfo.getConfig()) {
					parentInfo.setFlag(VersionInfo::IsDirect, parentInfo.getConfig()->getName() == currentInfo.getConfig()->getBranchNodeName());
				}
				else {
					parentInfo.setFlag(VersionInfo::IsDirect, false);
				}
			}
			isFirst = false;

			// Gather childs information
			VersionInfo childsInfo;

			// Check if child is end of branch
			if (currentInfo.getConfig() == &branchVersions.back()) {
				childsInfo.setFlag(VersionInfo::IsEndOfBranch, true);
			}
			else {
				// Check if next version is active
				auto it2 = versionIt;
				it2++;
				if (it2 != branchVersions.end()) {
					childsInfo.setFlag(VersionInfo::IsActive, it2->getName() == newConfig.getActiveVersionName());
				}
			}

			// Check if any child branch exists and if any of them start with the active version.
			for (const std::list<VersionGraphVersionCfg>& existingBranchVersions : m_config.getBranchesFromNode(currentInfo.getConfig()->getName())) {
				OTAssert(!existingBranchVersions.empty(), "Empty branch stored");
				if (existingBranchVersions.front().getName() == newConfig.getActiveVersionName()) {
					childsInfo.setFlag(VersionInfo::IsActive, true);
				}
				childsInfo.setFlag(VersionInfo::IsBranchNode, true);
			}

			// Check if the version should be added
			if (this->filterVersion(viewMode, newConfig.getActiveVersionName(), filterText, parentInfo, currentInfo, childsInfo)) {
				// Copy original config
				VersionGraphVersionCfg newVersion(*currentInfo.getConfig());

				// Set parent
				if (parentInfo.getConfig()) {
					newVersion.setParentVersion(parentInfo.getConfig()->getName());
				}
				else {
					newVersion.setParentVersion("");
				}
				newVersion.setDirectParentIsHidden(!(parentInfo.getFlags() & VersionInfo::IsDirect));

				// Add config
				newVersions.push_back(std::move(newVersion));

				// Update current parent info
				parentInfo = std::move(currentInfo);
				parentInfo.setFlag(VersionInfo::IsDirect, true);
			}
			else {
				// Version should not be added, only update current parent info
				parentInfo.setFlag(VersionInfo::IsDirect, false);
			}
		}

		// If any versions were added to the branch add the branch
		if (!newVersions.empty()) {
			newConfig.insertBranch(std::move(newVersions));
		}
	}

	// Apply new config
	m_graph->setupFromConfig(newConfig);
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

bool ot::VersionGraphManager::findParentVersionInOtherBranches(const VersionGraphVersionCfg& _version, const std::list<std::list<VersionGraphVersionCfg>>& _branches, VersionInfo& _parentInfo) {
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
						_parentInfo.setConfig(&otherVersion);
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
					_parentInfo.setConfig(lastVersion);
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

bool ot::VersionGraphManager::filterVersion(ViewMode _viewMode, const std::string& _activeVersionName, const std::string& _filterText, const VersionInfo& _parentInfo, const VersionInfo& _versionInfo, const VersionInfo& _childsInfo) {
	OTAssertNullptr(_versionInfo.getConfig());

	switch (_viewMode) {
	case ot::VersionGraphManager::ViewAll: return this->filterModeAll(_activeVersionName, _filterText, _parentInfo, _versionInfo, _childsInfo);
	case ot::VersionGraphManager::Compact: return this->filterModeCompact(_activeVersionName, _filterText, _parentInfo, _versionInfo, _childsInfo);
	case ot::VersionGraphManager::CompactLabeled: return this->filterModeCompactLabeled(_activeVersionName, _filterText, _parentInfo, _versionInfo, _childsInfo);
	case ot::VersionGraphManager::LabeledOnly: return this->filterModeLabeledOnly(_activeVersionName, _filterText, _parentInfo, _versionInfo, _childsInfo);
	case ot::VersionGraphManager::Iterator: return this->filterModeIterator(_activeVersionName, _filterText, _parentInfo, _versionInfo, _childsInfo);
	default:
		OT_LOG_EAS("Unknown view mode (" + std::to_string((int)_viewMode) + ")");
		return this->filterModeAll(_activeVersionName, _filterText, _parentInfo, _versionInfo, _childsInfo);
	}
}

bool ot::VersionGraphManager::filterModeAll(const std::string& _activeVersionName, const std::string& _filterText, const VersionInfo& _parentInfo, const VersionInfo& _versionInfo, const VersionInfo& _childsInfo) {
	if (_versionInfo.getConfig()->getName() == _activeVersionName || _filterText.empty() || _versionInfo.getConfig()->getName() == "1") {
		return true;
	}
	else {
		return this->checkTextFilter(_versionInfo, _filterText);
	}
}

bool ot::VersionGraphManager::filterModeCompact(const std::string& _activeVersionName, const std::string& _filterText, const VersionInfo& _parentInfo, const VersionInfo& _versionInfo, const VersionInfo& _childsInfo) {
	bool visibleDefault = _versionInfo.getConfig()->getName() == _activeVersionName || _versionInfo.getConfig()->getName() == "1";
	bool visibleNeighbour = _parentInfo.getFlags() & VersionInfo::IsActive || _childsInfo.getFlags() & VersionInfo::IsActive;
	bool visibleBranch = _versionInfo.getFlags() & VersionInfo::IsBranchNode || _versionInfo.getFlags() & VersionInfo::IsEndOfBranch;

	if (visibleDefault || visibleNeighbour || visibleBranch) {
		return this->checkTextFilter(_versionInfo, _filterText);
	}
	return false;
}

bool ot::VersionGraphManager::filterModeCompactLabeled(const std::string& _activeVersionName, const std::string& _filterText, const VersionInfo& _parentInfo, const VersionInfo& _versionInfo, const VersionInfo& _childsInfo) {
	bool visibleDefault = _versionInfo.getConfig()->getName() == _activeVersionName || _versionInfo.getConfig()->getName() == "1";
	bool visibleNeighbour = _parentInfo.getFlags() & VersionInfo::IsActive || _childsInfo.getFlags() & VersionInfo::IsActive;
	bool visibleBranch = _versionInfo.getFlags() & VersionInfo::IsBranchNode || _versionInfo.getFlags() & VersionInfo::IsEndOfBranch;
	bool visibleLabeled = !_versionInfo.getConfig()->getLabel().empty();

	if (visibleDefault || visibleNeighbour || visibleBranch || visibleLabeled) {
		return this->checkTextFilter(_versionInfo, _filterText);
	}
	return false;
}

bool ot::VersionGraphManager::filterModeLabeledOnly(const std::string& _activeVersionName, const std::string& _filterText, const VersionInfo& _parentInfo, const VersionInfo& _versionInfo, const VersionInfo& _childsInfo) {
	bool visibleDefault = _versionInfo.getConfig()->getName() == _activeVersionName || _versionInfo.getConfig()->getName() == "1";
	bool visibleLabeled = !_versionInfo.getConfig()->getLabel().empty();

	if (visibleDefault || visibleLabeled) {
		return this->checkTextFilter(_versionInfo, _filterText);
	}
	return false;
}

bool ot::VersionGraphManager::filterModeIterator(const std::string& _activeVersionName, const std::string& _filterText, const VersionInfo& _parentInfo, const VersionInfo& _versionInfo, const VersionInfo& _childsInfo) {
	bool visibleDefault = _versionInfo.getConfig()->getName() == _activeVersionName 
		|| _versionInfo.getConfig()->getName() == "1"
		|| ((_versionInfo.getFlags() & (VersionInfo::IsOnActivePath | VersionInfo::IsEndOfBranch)) == (VersionInfo::IsOnActivePath | VersionInfo::IsEndOfBranch));

	bool visibleNeighbour = _parentInfo.getFlags() & VersionInfo::IsActive 
		|| _childsInfo.getFlags() & VersionInfo::IsActive;

	if (visibleDefault || visibleNeighbour) {
		return true;
	}
	return false;
}

bool ot::VersionGraphManager::checkTextFilter(const VersionInfo& _versionInfo, const std::string& _filter) const {
	if (_filter.empty() || !_versionInfo.getConfig()) {
		return true;
	}

	QString filterText = QString::fromStdString(_filter).toLower();
	QString entry = QString::fromStdString(_versionInfo.getConfig()->getName()).toLower();
	if (entry.contains(filterText, Qt::CaseInsensitive)) {
		return true;
	}

	entry = QString::fromStdString(_versionInfo.getConfig()->getLabel()).toLower();
	if (entry.contains(filterText, Qt::CaseInsensitive)) {
		return true;
	}

	entry = QString::fromStdString(_versionInfo.getConfig()->getDescription()).toLower();
	if (entry.contains(filterText, Qt::CaseInsensitive)) {
		return true;
	}

	return false;
}
