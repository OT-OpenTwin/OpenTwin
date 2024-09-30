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

	class ComboBox;

	class OT_WIDGETS_API_EXPORT VersionGraphManager : public QObject, public QWidgetInterface {
		Q_OBJECT
	public:
		VersionGraphManager();
		virtual ~VersionGraphManager();

		virtual QWidget* getQWidget(void) override { return m_root; };

		VersionGraph* getGraph(void) { return m_graph; };
		const VersionGraph* getGraph(void) const { return m_graph; };

		void setupConfig(const VersionGraphCfg& _config);

		void addVersion(const std::string& _parentVersionName, const VersionGraphVersionCfg& _config);

		VersionGraphVersionCfg* addVersion(const ConstJsonObject& _versionConfig);

		void activateVersion(const std::string& _versionName, const std::string& _activeBranchVersionName);

		void removeVersion(const std::string& _versionName);

		void removeVersions(const std::list<std::string>& _versionNames);

	public Q_SLOTS:
		void updateCurrentGraph(void);

	private:
		void updateCurrentGraphCompactMode(void);
		void updateCurrentGraphCompactLabelMode(void);

		void startProcessCompact(bool _includeLabeledVersions);
		void processCompactItem(VersionGraphVersionCfg* _parent, const VersionGraphVersionCfg* _config, const std::string& _activeVersion, bool _isDirectParent, bool _includeLabeledVersions);

		QWidget* m_root;
		VersionGraph* m_graph;
		ComboBox* m_modeSelector;

		VersionGraphCfg m_config;
	};

}