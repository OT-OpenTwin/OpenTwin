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

void ot::GraphicsPickerManager::setCurrentKey(const std::string& _key) {
	if (_key == m_currentKey) {
		return;
	}

	this->clearPicker();

	m_currentKey = _key;

	this->applyCurrentKey();
}

void ot::GraphicsPickerManager::addCollections(const std::list<GraphicsPickerCollectionCfg>& _collections, const std::string& _key) {
	OTAssert(!_key.empty(), "No key provided");

	GraphicsPickerCollectionCfg tmpRoot;
	for (const GraphicsPickerCollectionCfg& c : _collections) {
		tmpRoot.addChildCollection(c);
	}
	this->getInfo(_key).collection.mergeWith(std::move(tmpRoot));

	if (_key == m_currentKey) {
		this->applyCurrentKey();
	}
}

void ot::GraphicsPickerManager::addCollections(std::list<GraphicsPickerCollectionCfg>&& _collections, const std::string& _key) {
	OTAssert(!_key.empty(), "No key provided");

	GraphicsPickerCollectionCfg tmpRoot;
	for (GraphicsPickerCollectionCfg& c : _collections) {
		tmpRoot.addChildCollection(std::move(c));
	}
	this->getInfo(_key).collection.mergeWith(std::move(tmpRoot));

	if (_key == m_currentKey) {
		this->applyCurrentKey();
	}
}

const std::list<ot::GraphicsPickerCollectionCfg>& ot::GraphicsPickerManager::getCollections(const std::string& _key) const {
	return this->getInfo(_key).collection.getChildCollections();
}

void ot::GraphicsPickerManager::clear() {
	m_collections.clear();
	m_currentKey.clear();
	
	this->clearPicker();
}

void ot::GraphicsPickerManager::clearPicker() {
	if (!m_picker) {
		return;
	}
	if (!m_currentKey.empty()) {
		this->getInfo(m_currentKey).state = m_picker->getCurrentState();
	}

	m_picker->clear();
	m_currentKey.clear();
	m_picker->setKey(m_currentKey);
}

void ot::GraphicsPickerManager::applyCurrentKey() {
	if (!m_picker) {
		return;
	}
	if (m_currentKey.empty()) {
		return;
	}

	m_picker->setKey(m_currentKey);
	
	PickerInfo& info = this->getInfo(m_currentKey);
	
	m_picker->add(info.collection.getChildCollections());
	m_picker->applyState(info.state);
}

ot::GraphicsPickerManager::PickerInfo& ot::GraphicsPickerManager::getInfo(const std::string& _key) {
	auto it = m_collections.find(_key);
	if (it == m_collections.end()) {
		return m_collections.insert_or_assign(_key, PickerInfo()).first->second;
	}
	else {
		return it->second;
	}
}

const ot::GraphicsPickerManager::PickerInfo& ot::GraphicsPickerManager::getInfo(const std::string& _key) const {
	auto it = m_collections.find(_key);
	if (it == m_collections.end()) {
		return m_emptyInfo;
	}
	else {
		return it->second;
	}
}