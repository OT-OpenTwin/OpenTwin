//! @file NavigationSelectionManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "NavigationSelectionManager.h"

// OpenTwin header
#include "OTCore/LocalStateStack.h"

ot::NavigationSelectionManager::NavigationSelectionManager() 
	: m_stateStack(ot::SelectionResult::Default)
{

}

ot::NavigationSelectionManager::~NavigationSelectionManager() {

}

ot::SelectionResultFlags ot::NavigationSelectionManager::runSelectionHandling(SelectionOrigin _eventOrigin, const UIDList& _newSelection) {
	m_previousSelectionInfo = m_selectionInfo;
	m_selectionInfo.setSelectedNavigationItems(_newSelection);

	UIDList diffList;
	for (UID uid : m_selectionInfo.getSelectedNavigationItems()) {
		bool found = false;
		for (UID prevUid : m_previousSelectionInfo.getSelectedNavigationItems()) {
			if (prevUid == uid) {
				found = true;
				break;
			}
		}

		if (!found) {
			diffList.push_back(uid);
		}
	}

	LocalStateStack<SelectionResultFlags> lclState(m_stateStack, SelectionResult::Default);

	Q_EMIT selectionHasChanged(*lclState, _eventOrigin);
	
	m_selectionInfo.addSelectedNavigationItems(std::move(diffList));

	return lclState.getCurrent();
}

void ot::NavigationSelectionManager::slotViewSelected(void) {
	m_stateStack->setFlag(SelectionResult::ActiveViewChanged);

}

void ot::NavigationSelectionManager::slotViewDeselected(void) {
	m_stateStack->setFlag(SelectionResult::ActiveViewChanged);

}
