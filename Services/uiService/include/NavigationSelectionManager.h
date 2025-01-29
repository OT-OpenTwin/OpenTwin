//! @file NavigationSelectionManager.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

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

		void addSelectedItem(UID _itemId) { m_selectionInfo.addSelectedNavigationItem(_itemId); };
		void setSelectedItems(const UIDList& _itemIds) { m_selectionInfo.setSelectedNavigationItems(_itemIds); };
		void setSelectionInformation(const SelectionInformation& _info) { m_selectionInfo = _info; };
		const SelectionInformation& getSelectionInformation(void) { return m_selectionInfo; };
		const UIDList& getSelectedItems(void) const { return m_selectionInfo.getSelectedNavigationItems(); };
		const SelectionInformation& getPreviousSelectionInformation(void) { return m_previousSelectionInfo; };
		const UIDList& getPreviouslySelectedItems(void) const { return m_previousSelectionInfo.getSelectedNavigationItems(); };

		bool isSelectionHandlingRunning(void) const { return m_runCounter > 0; };

	Q_SIGNALS:
		void selectionHasChanged(SelectionHandlingResult* _result, SelectionOrigin _eventOrigin);

	public Q_SLOTS:
		void slotViewSelected(void);
		void slotViewDeselected(void);

	private:
		int m_runCounter;
		StateStack<SelectionHandlingResult> m_stateStack;
		SelectionInformation m_selectionInfo;
		SelectionInformation m_previousSelectionInfo;
	};

}
