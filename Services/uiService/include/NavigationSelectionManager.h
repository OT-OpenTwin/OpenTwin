// @otlicense
// File: NavigationSelectionManager.h
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
#include "OTCore/StateStack.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/SelectionInformation.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {

	class NavigationSelectionManager : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(NavigationSelectionManager)
		OT_DECL_NOMOVE(NavigationSelectionManager)
	public:
		NavigationSelectionManager();
		virtual ~NavigationSelectionManager();

		SelectionHandlingResult runSelectionHandling(SelectionOrigin _eventOrigin, const UIDList& _newSelection);

		void addSelectedItem(UID _itemId);
		void setSelectedItems(const UIDList& _itemIds);
		void setSelectionInformation(const SelectionInformation& _info);
		const SelectionInformation& getSelectionInformation(void) { return m_selectionInfo; };
		const UIDList& getSelectedItems(void) const { return m_selectionInfo.getSelectedNavigationItems(); };
		const SelectionInformation& getPreviousSelectionInformation(void) { return m_previousSelectionInfo; };
		const UIDList& getPreviouslySelectedItems(void) const { return m_previousSelectionInfo.getSelectedNavigationItems(); };
		SelectionOrigin getCurrentSelectionOrigin(void) const { return m_selectionOrigin; };

		bool isSelectionHandlingRunning(void) const { return m_runCounter > 0; };

	Q_SIGNALS:
		void selectionHasChanged(SelectionHandlingResult* _result, SelectionOrigin _eventOrigin);

	public Q_SLOTS:
		void slotViewSelected(void);
		void slotViewDeselected(void);

	private:
		int m_runCounter;
		StateStack<SelectionHandlingResult> m_stateStack;
		SelectionOrigin m_selectionOrigin;
		SelectionInformation m_selectionInfo;
		SelectionInformation m_previousSelectionInfo;
	};

}
