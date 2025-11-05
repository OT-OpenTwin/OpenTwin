// @otlicense
// File: NavigationTreeView.h
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
#include "OTGui/CopyInformation.h"
#include "OTWidgets/WidgetView.h"

namespace ak { class aTreeWidget; };

namespace ot {

	class NavigationTreeView : public WidgetView {
		Q_OBJECT
		OT_DECL_NOCOPY(NavigationTreeView)
		OT_DECL_NOMOVE(NavigationTreeView)
		OT_DECL_NODEFAULT(NavigationTreeView)
	public:
		explicit NavigationTreeView(QWidget* _parent);
		virtual ~NavigationTreeView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		ak::aTreeWidget* getTree(void) const { return m_tree; };

	Q_SIGNALS:
		void copyRequested(const ot::CopyInformation& _info);
		void pasteRequested(const ot::CopyInformation& _info);

	private Q_SLOTS:
		void slotCopyRequested(void);
		void slotPasteRequested(void);

	private:
		ak::aTreeWidget* m_tree;
	};

}
