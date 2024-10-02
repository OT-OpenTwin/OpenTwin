//! @file VersionGraphItem.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/VersionGraphVersionCfg.h"
#include "OTWidgets/GraphicsStackItem.h"

namespace ot {

	class GraphicsTextItem;
	class GraphicsInvisibleItem;
	class GraphicsConnectionItem;
	
	class OT_WIDGETS_API_EXPORT VersionGraphItem : public GraphicsStackItem {
		OT_DECL_NOCOPY(VersionGraphItem)
		OT_DECL_NODEFAULT(VersionGraphItem)
	public:
		VersionGraphItem(const VersionGraphVersionCfg& _config, int _row, const std::string& _activeVersion, GraphicsScene* _scene);
		virtual ~VersionGraphItem();

		const VersionGraphVersionCfg& getVersionConfig(void) const { return m_config; };

		void setParentVersionItem(VersionGraphItem* _item) { m_parentVersion = _item; };
		VersionGraphItem* getParentVersionItem(void) const { return m_parentVersion; };

		void setRowIndex(int _row) { m_row = _row; };
		int getRowIndex(void) const { return m_row; };
		//! \brief Returns the maximum row index of this version and all of its childs.
		int getMaxRowIndex(void) const;

		void addChildVersion(VersionGraphItem* _version);
		void forgetChildVersion(VersionGraphItem* _version);
		std::list<VersionGraphItem*> getChildVersions(void) const { return m_childVersions; };

		void connectToParent(void);

		VersionGraphItem* findVersionByName(const std::string& _versionName);

		void updateVersionPosition(void);

		void setAsActiveVersionBranch(void);

		const QSizeF& getCurrentSize(void) const { return m_currentSize; };

	private:
		void updateToRootAsActiveBranch(void);
		
		VersionGraphVersionCfg m_config;
		VersionGraphItem* m_parentVersion;
		GraphicsConnectionItem* m_parentConnection;
		GraphicsTextItem* m_nameItem;
		GraphicsTextItem* m_labelItem;
		GraphicsInvisibleItem* m_inConnector;
		GraphicsInvisibleItem* m_outConnector;
		
		std::list<VersionGraphItem*> m_childVersions;
		int m_row;
		QSizeF m_currentSize;
	};

}
