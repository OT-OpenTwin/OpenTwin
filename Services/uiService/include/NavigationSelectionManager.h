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

		SelectionResultFlags runSelectionHandling(SelectionOrigin _eventOrigin, const UIDList& _newSelection);

		void setSelectedItems(const UIDList& _itemIds) { m_selectionInfo.setSelectedNavigationItems(_itemIds); };
		void setSelectionInformation(const SelectionInformation& _info) { m_selectionInfo = _info; };
		const SelectionInformation& getSelectionInformation(void) { return m_selectionInfo; };
		const UIDList& getSelectedItems(void) const { return m_selectionInfo.getSelectedNavigationItems(); };
		const SelectionInformation& getPreviousSelectionInformation(void) { return m_previousSelectionInfo; };
		const UIDList& getPreviouslySelectedItems(void) const { return m_previousSelectionInfo.getSelectedNavigationItems(); };

	Q_SIGNALS:
		void selectionHasChanged(SelectionResultFlags* _result, SelectionOrigin _eventOrigin);

	public Q_SLOTS:
		void slotViewSelected(void);
		void slotViewDeselected(void);

	private:
		StateStack<SelectionResultFlags> m_stateStack;
		SelectionInformation m_selectionInfo;
		SelectionInformation m_previousSelectionInfo;
	};

}
