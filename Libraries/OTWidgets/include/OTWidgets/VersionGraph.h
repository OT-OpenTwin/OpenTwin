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
		VersionGraph();
		virtual ~VersionGraph();

		void setupFromConfig(const VersionGraphCfg& _config);
		
		void clear(void);

		bool isCurrentVersionEndOfBranch(void) const;
		bool isVersionIsEndOfBranch(const std::string& _versionName) const;

	Q_SIGNALS:
		void versionDeselected(void);
		void versionSelected(const std::string& _versionName);
		void versionActivatRequest(const std::string& _versionName);

	protected:
		virtual void showEvent(QShowEvent* _event) override;

	public Q_SLOTS:
		void slotSelectionChanged(void);
		void slotUpdateVersionItems(void);
		void slotCenterOnActiveVersion(void);
		void slotGraphicsItemDoubleClicked(const ot::GraphicsItem* _item);

	private:
		QRectF calculateFittedViewportRect(void) const;
		void centerOnVersion(const QRectF& _oldRect);
		VersionGraphItem* getVersion(const std::string& _name) const;
		void highlightVersion(const std::string& _name);

		QRectF m_lastViewportRect;
		std::string m_activeVersion;
		std::string m_activeVersionBranch;
		VersionGraphItem* m_rootItem;

	};

}

