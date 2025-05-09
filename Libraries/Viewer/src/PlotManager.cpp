//! @file PlotManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "PlotManager.h"
#include "DataBase.h"
#include "EntityHandler.h"
#include "OTWidgets/PlotDataset.h"

ot::PlotManager::PlotManager() {

}

ot::PlotManager::~PlotManager() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

ot::PlotDataset* ot::PlotManager::findDataset(QwtPlotCurve* _curve) {
	for (auto itm : m_cache) {
		if (itm.second.second->getCartesianCurve() == _curve) {
			return itm.second.second;
		}
	}
	return nullptr;
}

ot::PlotDataset* ot::PlotManager::findDataset(QwtPolarCurve* _curve) {
	for (auto itm : m_cache) {
		if (itm.second.second->getPolarCurve() == _curve) {
			return itm.second.second;
		}
	}
	return nullptr;
}

std::list<ot::PlotDataset*> ot::PlotManager::findDatasets(UID _entityID) {
	std::list<ot::PlotDataset*> result;
	auto it = m_cache.find(_entityID);
	if (it != m_cache.end()) {
		result.push_back(it->second.second);
	}
	return result;
}

std::list<ot::PlotDataset*> ot::PlotManager::getAllDatasets(void) const {
	std::list<PlotDataset*> result;

	for (const auto& it : m_cache) {
		result.push_back(it.second.second);
	}

	return result;
}

void ot::PlotManager::addDatasetToCache(PlotDataset* _dataset) {
	m_cache.insert_or_assign(
		_dataset->getEntityID(),
		std::pair<UID, PlotDataset*>(
			_dataset->getEntityVersion(),
			_dataset
		)
	);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data handling

void ot::PlotManager::removeFromCache(unsigned long long _entityID) {
	auto itm = m_cache.find(_entityID);
	if (itm != m_cache.end()) {
		delete itm->second.second;
		m_cache.erase(_entityID);
	}
}

bool ot::PlotManager::hasCachedEntity(UID _entityID) const {
	return m_cache.find(_entityID) != m_cache.end();
}

bool ot::PlotManager::changeCachedDatasetEntityVersion(UID _entityID, UID _newEntityVersion) {
	auto it = m_cache.find(_entityID);
	if (it == m_cache.end()) {
		return false;
	}

	if (it->second.first == _newEntityVersion) {
		return false;
	}
	else {
		it->second.first = _newEntityVersion;
		it->second.second->setEntityVersion(_newEntityVersion);
		return true;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected virtual methods

void ot::PlotManager::clearCache(void) {
	for (auto itm : m_cache) {
		delete itm.second.second;
	}
	m_cache.clear();
}

void ot::PlotManager::detachAllCached(void) {
	for (auto itm : m_cache) {
		itm.second.second->detach();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private
