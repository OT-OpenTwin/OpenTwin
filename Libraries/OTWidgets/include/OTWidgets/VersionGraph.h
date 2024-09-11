//! @file VersionGraph.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/VersionGraphCfg.h"
#include "OTWidgets/GraphicsView.h"

// std header
#include <list>

namespace ot {

	class VersionGraphItem;

	class OT_WIDGETS_API_EXPORT VersionGraph : public GraphicsView {
		Q_OBJECT
	public:
		VersionGraph();
		virtual ~VersionGraph();

		void setupFromConfig(const VersionGraphCfg& _config);
		
		void clear(void);

	public Q_SLOTS:
		void slotUpdateVersionItems(void);
		void slotCenterOnVersion(const std::string& _versionName);

	private:
		VersionGraphItem* getVersion(const std::string& _name);
		void highlightVersion(const std::string& _name);

		std::list<VersionGraphItem*> m_rootItems;

	};

}

