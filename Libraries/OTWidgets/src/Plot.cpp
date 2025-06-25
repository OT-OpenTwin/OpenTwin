//! @file PlotManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Plot.h"
#include "OTWidgets/PlotDataset.h"
#include "OTWidgets/PolarPlotCurve.h"
#include "OTWidgets/CartesianPlotCurve.h"

ot::Plot::Plot() {

}

ot::Plot::~Plot() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

ot::PlotDataset* ot::Plot::findDataset(QwtPlotCurve* _curve) {
	for (auto itm : m_cache) {
		for (PlotDataset* dataset : itm.second) {
			if (dataset->getCartesianCurve() == _curve) {
				return dataset;
			}
		}
	}
	return nullptr;
}

ot::PlotDataset* ot::Plot::findDataset(QwtPolarCurve* _curve) {
	for (auto itm : m_cache) {
		for (PlotDataset* dataset : itm.second) {
			if (dataset->getPolarCurve() == _curve) {
				return dataset;
			}
		}
	}
	return nullptr;
}

std::list<ot::PlotDataset*> ot::Plot::findDatasets(UID _entityID) {
	auto it = m_cache.find(_entityID);
	if (it == m_cache.end()) {
		return std::list<PlotDataset*>();
	}
	else {
		return it->second;
	}
}

std::list<ot::PlotDataset*> ot::Plot::getAllDatasets(void) const {
	std::list<PlotDataset*> result;

	for (const auto& it : m_cache) {
		result.splice(result.end(), std::list<PlotDataset*>(it.second));
	}

	return result;
}

void ot::Plot::addDatasetToCache(PlotDataset* _dataset) {
	OTAssertNullptr(_dataset);
	auto it = m_cache.find(_dataset->getEntityID());

	if (it == m_cache.end()) {
		m_cache.insert_or_assign(_dataset->getEntityID(), std::list<PlotDataset*>({ _dataset }));
	}
	else {
		OTAssert(std::find(it->second.begin(), it->second.end(), _dataset) == it->second.end(), "Dataset already stored");
		it->second.push_back(_dataset);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data handling

void ot::Plot::removeFromCache(UID _entityID) {
	auto it = m_cache.find(_entityID);
	if (it != m_cache.end()) {
		for (PlotDataset* dataset : it->second) {
			OTAssertNullptr(dataset);
			delete dataset;
		}
		m_cache.erase(it);
	}
}

bool ot::Plot::hasCachedEntity(UID _entityID) const {
	return m_cache.find(_entityID) != m_cache.end();
}

bool ot::Plot::changeCachedDatasetEntityVersion(UID _entityID, UID _newEntityVersion) {
	bool changed = false;

	auto it = m_cache.find(_entityID);
	if (it != m_cache.end()) {
		for (PlotDataset* dataset : it->second) {
			if (dataset->getEntityVersion() != _newEntityVersion) {
				dataset->setEntityVersion(_newEntityVersion);
				changed = true;
			}
		}
	}

	return changed;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected virtual methods

void ot::Plot::clearCache(void) {
	for (auto it : m_cache) {
		for (PlotDataset* dataset : it.second) {
			OTAssertNullptr(dataset);
			delete dataset;
		}
	}
	m_cache.clear();
}

void ot::Plot::detachAllCached(void) {
	for (auto it : m_cache) {
		for (PlotDataset* dataset : it.second) {
			OTAssertNullptr(dataset);
			dataset->detach();
		}
	}
}
