//! @file GraphicsPickerCollectionManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsPickerCollectionManager.h"

void ot::GraphicsPickerCollectionManager::addCollection(const GraphicsPickerCollectionCfg& _collection, const BasicServiceInformation& _owner) {
	GraphicsPickerCollectionCfg tmpRoot;
	tmpRoot.addChildCollection(new GraphicsPickerCollectionCfg(_collection));
	this->getOwnerRootCollection(_owner).mergeWith(tmpRoot);
}

void ot::GraphicsPickerCollectionManager::addCollections(const std::list<GraphicsPickerCollectionCfg*>& _collections, const BasicServiceInformation& _owner) {
	GraphicsPickerCollectionCfg tmpRoot;
	for (const GraphicsPickerCollectionCfg* c : _collections) {
		tmpRoot.addChildCollection(new GraphicsPickerCollectionCfg(*c));
	}
	this->getOwnerRootCollection(_owner).mergeWith(tmpRoot);
}

const std::list<ot::GraphicsPickerCollectionCfg*>& ot::GraphicsPickerCollectionManager::getCollections(const BasicServiceInformation& _owner) const {
	return this->getOwnerRootCollection(_owner).getChildCollections();
}

void ot::GraphicsPickerCollectionManager::clear(void) {
	m_collections.clear();
	m_currentOwner = BasicServiceInformation();
}

ot::GraphicsPickerCollectionCfg& ot::GraphicsPickerCollectionManager::getOwnerRootCollection(const BasicServiceInformation& _owner) {
	auto it = m_collections.find(_owner);
	if (it == m_collections.end()) {
		m_collections.insert_or_assign(_owner, GraphicsPickerCollectionCfg());
		return this->getOwnerRootCollection(_owner);
	}
	else {
		return it->second;
	}
}

const ot::GraphicsPickerCollectionCfg& ot::GraphicsPickerCollectionManager::getOwnerRootCollection(const BasicServiceInformation& _owner) const {
	auto it = m_collections.find(_owner);
	if (it == m_collections.end()) {
		return m_emptyCollection;
	}
	else {
		return it->second;
	}
}