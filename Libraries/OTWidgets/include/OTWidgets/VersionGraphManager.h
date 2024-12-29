//! @file VersionGraphManager.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/VersionGraph.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {

	class LineEdit;
	class ComboBox;

	class OT_WIDGETS_API_EXPORT VersionGraphManager : public QObject, public QWidgetInterface {
		Q_OBJECT
	public:
		enum ViewMode {
			ViewAll,
			Compact,
			CompactLabeled,
			LabeledOnly
		};
		static std::string viewModeToString(ViewMode _mode);
		static ViewMode stringToViewMode(const std::string& _mode);

		VersionGraphManager();
		virtual ~VersionGraphManager();

		virtual QWidget* getQWidget(void) override { return m_root; };

		VersionGraph* getGraph(void) { return m_graph; };
		const VersionGraph* getGraph(void) const { return m_graph; };

		void setupConfig(VersionGraphCfg&& _config);

		void setCurrentViewMode(ViewMode _mode);
		ViewMode getCurrentViewMode(void) const;

		VersionGraphVersionCfg& addVersion(VersionGraphVersionCfg&& _config);

		VersionGraphVersionCfg* addVersion(const ConstJsonObject& _versionConfig);

		void activateVersion(const std::string& _versionName, const std::string& _activeBranchVersionName);

		void removeVersion(const std::string& _versionName);

		void removeVersions(const std::list<std::string>& _versionNames);

		bool versionIsBranchNode(const std::string& _versionName);

	public Q_SLOTS:
		void updateCurrentGraph(void);

	private:
		enum class ParentFlag {
			None = 0 << 0,
			IsActive = 1 << 0,
			IsDirect = 1 << 1,
			IsBranchNode = 1 << 2
		};
		typedef Flags<ParentFlag> ParentFlags;

		struct ParentInfo {
			ParentInfo() :
				parent(nullptr),
				flags(ParentFlag::None)
			{};

			const VersionGraphVersionCfg* parent;
			ParentFlags flags;
		};

		void updateCurrentGraphViewAllMode(void);
		void updateCurrentGraphCompactMode(void);
		void updateCurrentGraphCompactLabelMode(void);
		void updateCurrentGraphLabeledOnlyMode(void);

		//! @brief Returns true if the version number of _left is greater than _right (only concidering branch version number)
		//! @param _left Greater value
		//! @param _right Lower equal value
		bool isVersionGreater(const std::string& _left, const std::string& _right) const;

		bool findParentVersionInOtherBranches(const VersionGraphVersionCfg& _version, const std::list<std::list<VersionGraphVersionCfg>>& _branches, const VersionGraphVersionCfg*& _parent);

		//! \brief Returns true if the filter matches the version or the filter is empty
		bool checkFilterValid(const VersionGraphVersionCfg* _versionConfig, const QString& _filterText) const;

		//! Allows all versions to be shown.
		//! If a text filter is set the filter will be used case insensitive in the version name, label and description.
		//! Version "1" is always shown.
		bool filterModeAll(const VersionGraphVersionCfg& _thisVersion, const std::string& _activeVersionName, const std::string& _filterText, const ParentInfo& _parentInfo);

		/*void startProcessCompact(bool _includeLabeledVersions, const QString& _filterText);
		void processCompactItem(VersionGraphVersionCfg* _parent, const VersionGraphVersionCfg* _config, const std::string& _activeVersion, bool _isDirectParent, bool _includeLabeledVersions, const QString& _filterText);
		void processLabeledOnlyItem(VersionGraphVersionCfg* _parent, const VersionGraphVersionCfg* _config, const std::string& _activeVersion, bool _isDirectParent, const QString& _filterText);
		*/
		QWidget* m_root;
		VersionGraph* m_graph;
		LineEdit* m_textFilter;
		ComboBox* m_modeSelector;

		VersionGraphCfg m_config;

		OT_ADD_FRIEND_FLAG_FUNCTIONS(ParentFlag)
	};

}