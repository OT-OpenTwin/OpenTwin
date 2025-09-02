//! @file NavigationSelectionManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "NavigationSelectionManager.h"

// OpenTwin header
#include "OTCore/ContainerHelper.h"
#include "OTCore/LocalStateStack.h"
#include "OTCore/ContainerHelper.h"
#include "OTCore/BasicNumberIncrementWrapper.h"

ot::NavigationSelectionManager::NavigationSelectionManager() 
	: m_stateStack(SelectionHandlingEvent::Default), m_selectionOrigin(SelectionOrigin::Custom), m_runCounter(0)
{

}

ot::NavigationSelectionManager::~NavigationSelectionManager() {

}

ot::SelectionHandlingResult ot::NavigationSelectionManager::runSelectionHandling(SelectionOrigin _eventOrigin, const UIDList& _newSelection) {
	if (ContainerHelper::isEqual(_newSelection, m_selectionInfo.getSelectedNavigationItems())) {
		// No change
		return SelectionHandlingEvent::Default;
	}

	BasicNumberIncrementWrapper ct(m_runCounter);

	m_selectionOrigin = _eventOrigin;
	m_previousSelectionInfo = m_selectionInfo;
	m_selectionInfo.setSelectedNavigationItems(_newSelection);

	LocalStateStack<SelectionHandlingResult> lclState(m_stateStack, SelectionHandlingEvent::Default);

	Q_EMIT selectionHasChanged(*lclState, _eventOrigin);
	
	m_selectionOrigin = _eventOrigin;
	return lclState.getCurrent();
}

void ot::NavigationSelectionManager::addSelectedItem(UID _itemId) {
	m_selectionInfo.addSelectedNavigationItem(_itemId);
}

void ot::NavigationSelectionManager::setSelectedItems(const UIDList& _itemIds) {
	m_selectionInfo.setSelectedNavigationItems(_itemIds);
}

void ot::NavigationSelectionManager::setSelectionInformation(const SelectionInformation& _info) {
	m_selectionInfo = _info;
}

void ot::NavigationSelectionManager::slotViewSelected(void) {
	m_stateStack->setFlag(SelectionHandlingEvent::ActiveViewChanged);
}

void ot::NavigationSelectionManager::slotViewDeselected(void) {
	m_stateStack->setFlag(SelectionHandlingEvent::ActiveViewChanged);
}
