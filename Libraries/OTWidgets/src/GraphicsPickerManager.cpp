// @otlicense
// File: GraphicsPickerManager.cpp
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
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/GraphicsPickerManager.h"

ot::GraphicsPickerManager::GraphicsPickerManager(GraphicsPicker* _picker) 
	: m_picker(_picker)
{

}

void ot::GraphicsPickerManager::setCurrentOwner(const BasicServiceInformation& _owner) {
	this->clearPicker();

	if (_owner == m_currentOwner) {
		return;
	}

	m_currentOwner = _owner;

	this->applyCurrentOwner();
}

void ot::GraphicsPickerManager::addCollections(const std::list<GraphicsPickerCollectionCfg>& _collections, const BasicServiceInformation& _owner) {
	OTAssert(_owner != BasicServiceInformation(), "No owner provided");

	GraphicsPickerCollectionCfg tmpRoot;
	for (const GraphicsPickerCollectionCfg& c : _collections) {
		tmpRoot.addChildCollection(c);
	}
	this->getOwnerInfo(_owner).collection.mergeWith(std::move(tmpRoot));

	if (_owner == m_currentOwner) {
		this->applyCurrentOwner();
	}
}

void ot::GraphicsPickerManager::addCollections(std::list<GraphicsPickerCollectionCfg>&& _collections, const BasicServiceInformation& _owner) {
	OTAssert(_owner != BasicServiceInformation(), "No owner provided");

	GraphicsPickerCollectionCfg tmpRoot;
	for (GraphicsPickerCollectionCfg& c : _collections) {
		tmpRoot.addChildCollection(std::move(c));
	}
	this->getOwnerInfo(_owner).collection.mergeWith(std::move(tmpRoot));

	if (_owner == m_currentOwner) {
		this->applyCurrentOwner();
	}
}

const std::list<ot::GraphicsPickerCollectionCfg>& ot::GraphicsPickerManager::getCollections(const BasicServiceInformation& _owner) const {
	return this->getOwnerInfo(_owner).collection.getChildCollections();
}

void ot::GraphicsPickerManager::clear() {
	m_collections.clear();
	m_currentOwner = BasicServiceInformation();
	
	this->clearPicker();
}

void ot::GraphicsPickerManager::clearPicker() {
	if (!m_picker) {
		return;
	}
	if (m_currentOwner != BasicServiceInformation()) {
		this->getOwnerInfo(m_currentOwner).state = m_picker->getCurrentState();
	}

	m_picker->clear();
	m_picker->setOwner(ot::BasicServiceInformation());
	m_currentOwner = ot::BasicServiceInformation();
}

void ot::GraphicsPickerManager::applyCurrentOwner() {
	if (!m_picker) {
		return;
	}
	if (m_currentOwner == BasicServiceInformation()) {
		return;
	}

	m_picker->setOwner(m_currentOwner);
	
	PickerInfo& info = this->getOwnerInfo(m_currentOwner);
	
	m_picker->add(info.collection.getChildCollections());
	m_picker->applyState(info.state);
}

ot::GraphicsPickerManager::PickerInfo& ot::GraphicsPickerManager::getOwnerInfo(const BasicServiceInformation& _owner) {
	auto it = m_collections.find(_owner);
	if (it == m_collections.end()) {
		return m_collections.insert_or_assign(_owner, PickerInfo()).first->second;
	}
	else {
		return it->second;
	}
}

const ot::GraphicsPickerManager::PickerInfo& ot::GraphicsPickerManager::getOwnerInfo(const BasicServiceInformation& _owner) const {
	auto it = m_collections.find(_owner);
	if (it == m_collections.end()) {
		return m_emptyInfo;
	}
	else {
		return it->second;
	}
}