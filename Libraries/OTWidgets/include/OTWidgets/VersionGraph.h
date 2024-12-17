//! @file VersionGraph.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/VersionGraphCfg.h"
#include "OTWidgets/GraphicsView.h"

namespace ot {

	class VersionGraphItem;

	class OT_WIDGETS_API_EXPORT VersionGraph : public GraphicsView {
		Q_OBJECT
	public:
		enum VersionGraphConfig {
			NoConfigFlags                     = 0 << 0, //! \brief No VersionGraph configuration flags.
			IgnoreActivateRequestOnReadOnly   = 1 << 0, //! \brief Activate version requests will be suppressed if the VersionGraph is in read-only state.
			IgnoreSelectionHandlingOnReadOnly = 1 << 1  //! \brief Version selection changed signals will be suppressed if the VersionGraph is in read-only state.
		};
		typedef Flags<VersionGraphConfig> VersionGraphConfigFlags;

		VersionGraph();
		virtual ~VersionGraph();

		void setupFromConfig(const VersionGraphCfg& _config);
		
		void clear(void);

		bool isCurrentVersionEndOfBranch(void) const;
		bool isVersionIsEndOfBranch(const std::string& _versionName) const;

		void setVersionGraphConfig(VersionGraphConfig _config, bool _active = true) { m_configFlags.setFlag(_config, _active); };
		void setVersionGraphConfigFlags(const VersionGraphConfigFlags& _flags) { m_configFlags = _flags; };
		const VersionGraphConfigFlags& getVersionGraphConfigFlags(void) const { return m_configFlags; };

	Q_SIGNALS:
		void versionDeselected(void);
		void versionSelected(const std::string& _versionName);
		void versionActivateRequest(const std::string& _versionName);

	public Q_SLOTS:
		void slotSelectionChanged(void);
		void slotCenterOnActiveVersion(void);
		void slotGraphicsItemDoubleClicked(const ot::GraphicsItem* _item);
		
	protected:
		virtual void showEvent(QShowEvent* _event) override;
		virtual void paintEvent(QPaintEvent* _event) override;

	private:
		void updateVersionPositions(void);
		QRectF calculateFittedViewportRect(void) const;
		VersionGraphItem* getVersion(const std::string& _name) const;
		void highlightVersion(const std::string& _name);

		bool m_updateItemPositionRequired;
		QRectF m_lastViewportRect;
		std::string m_activeVersion;
		std::string m_activeVersionBranch;
		VersionGraphItem* m_rootItem;
		VersionGraphConfigFlags m_configFlags;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::VersionGraph::VersionGraphConfig)