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

	};

}

