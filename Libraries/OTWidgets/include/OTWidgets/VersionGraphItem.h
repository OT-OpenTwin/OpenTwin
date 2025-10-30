// @otlicense
// File: VersionGraphItem.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
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
		//! @brief Returns the maximum row index of this version and all of its childs.
		int getMaxRowIndex(void) const;

		//! @param _version Version to add. Caller keeps ownership of the pointer.
		void addChildVersion(VersionGraphItem* _version);

		//! @brief Remove version from list.
		void forgetChildVersion(VersionGraphItem* _version);
		const std::list<VersionGraphItem*>& getChildVersions(void) const { return m_childVersions; };
		void clearChildVersions(void) { m_childVersions.clear(); };

		void connectToParent(void);
		void disconnectFromParent(void);

		void updateVersionPositionAndSize(void);

		void setIsInActiveBranch(void);

		const QSizeF& getCurrentSize(void) const { return m_currentSize; };

	private:
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
