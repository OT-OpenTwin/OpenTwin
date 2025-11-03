// @otlicense
// File: NavigationSelectionManager.cpp
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
	m_stateStack->set(SelectionHandlingEvent::ActiveViewChanged);
}

void ot::NavigationSelectionManager::slotViewDeselected(void) {
	m_stateStack->set(SelectionHandlingEvent::ActiveViewChanged);
}
