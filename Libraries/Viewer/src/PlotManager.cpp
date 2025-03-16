//! @file PlotManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "PlotManager.h"
#include "DataBase.h"
#include "EntityHandler.h"
#include "EntityResult1DCurve.h"
#include "EntityResult1DCurveData.h"
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

ot::PlotDataset* ot::PlotManager::findDataset(UID _entityID) {
	auto it = m_cache.find(_entityID);
	if (it == m_cache.end()) {
		return nullptr;
	}
	else {
		return it->second.second;
	}
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

void ot::PlotManager::setFromDataBaseConfig(const Plot1DDataBaseCfg& _config) {
	// Detach all current data and refresh the title
	this->clear(false);

	{
		Plot1DCfg cfg = _config;
		this->replaceConfig(std::move(cfg));
	}

	// Check cache
	std::list<Plot1DCurveCfg> entitiesToImport;
	for (const Plot1DCurveCfg& curveInfo : _config.getCurves()) {
		// Check for entity ID
		auto itm = m_cache.find(curveInfo.getEntityID());
		if (itm != m_cache.end()) {
			// Check if the entity Versions match
			if (itm->second.first != curveInfo.getEntityVersion()) {
				entitiesToImport.push_back(curveInfo);
			}
			else {
				itm->second.second->setNavigationId(curveInfo.getNavigationId());  // We need to update the tree id, since this might have changed due to undo / redo or open project
				itm->second.second->setDimmed(curveInfo.getDimmed(), true);
				itm->second.second->attach();
			}
		}
		else {
			entitiesToImport.push_back(curveInfo);
		}
	}

	// Import remaining entities
	this->importData(_config.getProjectName(), entitiesToImport);

	this->applyConfig();
}

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

void ot::PlotManager::importData(const std::string& _projectName, const std::list<Plot1DCurveCfg>& _entitiesToImport) {
	if (_entitiesToImport.empty()) return;

	std::list<std::pair<UID, UID>> prefetchCurves;
	for (const Plot1DCurveCfg& curveCfg : _entitiesToImport) {
		prefetchCurves.push_back(std::pair<UID, UID>(curveCfg.getEntityID(), curveCfg.getEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchCurves);

	std::list<std::pair<UID, UID>> prefetchCurveData;
	std::list< PlotDataset*> curves;

	for (const Plot1DCurveCfg& curveCfg : _entitiesToImport) {
		EntityHandler handler;
		EntityBase* baseEntity = handler.readEntityFromEntityIDandVersion(curveCfg.getEntityID(), curveCfg.getEntityVersion());
		std::unique_ptr<EntityResult1DCurve> curve(dynamic_cast<EntityResult1DCurve*>(baseEntity));

		// Read the curve data item
		UID curveDataStorageId = curve->getCurveDataStorageId();
		UID curveDataStorageVersion = curve->getCurveDataStorageVersion();
		baseEntity = handler.readEntityFromEntityIDandVersion(curveDataStorageId, curveDataStorageVersion);
		std::unique_ptr<EntityResult1DCurveData> curveData(dynamic_cast<EntityResult1DCurveData*>(baseEntity));
		
		Plot1DCurveCfg cfg = curveCfg;

		cfg.setTitle(curve->getName());

		cfg.setLinePenColor(curve->getColor());
		
		cfg.setXAxisUnit(curve->getUnitX());
		cfg.setYAxisUnit(curve->getUnitY());

		cfg.setXAxisTitle(curve->getAxisLabelX() + " [" + curve->getUnitX() + "]");
		cfg.setYAxisTitle(curve->getAxisLabelY() + " [" + curve->getUnitY() + "]");

		PlotDataset* newDataset = addDataset(cfg, nullptr, nullptr, 0);
		newDataset->setStorageEntityInfo(BasicEntityInformation(curve->getCurveDataStorageId(), curve->getCurveDataStorageVersion()));
		newDataset->updateCurveVisualization();

		// Check whether we already have the curve data

		bool loadCurveDataRequired = true;

		auto it = m_cache.find(newDataset->getEntityID());
		if (it != m_cache.end()) {
			PlotDataset* oldDataset = it->second.second;

			if (oldDataset != nullptr) {
				if (oldDataset->getStorageEntityInfo().getEntityID() == newDataset->getStorageEntityInfo().getEntityID()
					&& oldDataset->getStorageEntityInfo().getEntityVersion() == newDataset->getStorageEntityInfo().getEntityVersion()) {
					
					// The curve data in the previous data set is the same as the one in the new data set
					double* x = nullptr, * y = nullptr;
					long n;

					if (oldDataset->getCopyOfData(x, y, n)) {
						newDataset->replaceData(x, y, n);
						loadCurveDataRequired = false;
						newDataset->attach();
					}
				}

			}
		}

		if (loadCurveDataRequired) {
			prefetchCurveData.push_back(std::pair<UID, UID>(curveDataStorageId, curveDataStorageVersion));

			curves.push_back(newDataset);
		}

		removeFromCache(newDataset->getEntityID());

		this->addDatasetToCache(newDataset);
	}

	if (prefetchCurveData.empty()) return; // Nothing to load

	// Now load the curve data
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchCurveData);

	for (PlotDataset* item : curves) {
		// Here we get the storage data of the underlying curve data
		UID entityID = item->getStorageEntityInfo().getEntityID();
		UID entityVersion = item->getStorageEntityInfo().getEntityVersion();

		auto doc = bsoncxx::builder::basic::document{};

		if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(entityID, entityVersion, doc)) {
			assert(0);
			return;
		}

		auto doc_view = doc.view()["Found"].get_document().view();

		std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

		EntityResult1DCurveData curveDataEntName(0, nullptr, nullptr, nullptr, nullptr, "");
		if (entityType != curveDataEntName.getClassName()) {
			assert(0);
			return;
		}

		std::string schemaVersionField = "SchemaVersion_" + curveDataEntName.getClassName();
		int schemaVersion = (int)DataBase::GetIntFromView(doc_view, schemaVersionField.c_str());
		if (schemaVersion != 1) {
			assert(0);
			return;
		}

		auto arrayX = doc_view["dataX"].get_array().value;
		auto arrayYre = doc_view["dataY"].get_array().value;
		auto arrayYim = doc_view["dataYim"].get_array().value;

		size_t nX = std::distance(arrayX.begin(), arrayX.end());
		size_t nYre = std::distance(arrayYre.begin(), arrayYre.end());
		size_t nYim = std::distance(arrayYim.begin(), arrayYim.end());

		double* x = new double[nX];
		auto iX = arrayX.begin();

		for (unsigned long index = 0; index < nX; index++) {
			x[index] = iX->get_double();
			iX++;
		}

		double* yre = nullptr, * yim = nullptr;

		if (nYre > 0) {
			yre = new double[nYre];

			auto iYre = arrayYre.begin();

			for (unsigned long index = 0; index < nYre; index++) {
				yre[index] = iYre->get_double();
				iYre++;
			}
		}
		else {
			yre = new double[nX];
			for (unsigned long index = 0; index < nX; index++) {
				yre[index] = 0;
			}
		}

		// First we set the new data
		item->replaceData(x, yre, nX);

		if (nYim > 0) {
			yim = new double[nYim];

			auto iYim = arrayYim.begin();

			for (unsigned long index = 0; index < nYim; index++) {
				yim[index] = iYim->get_double();
				iYim++;
			}

			// Set the imaginary values
			assert(nX == nYre);
		}
		else {
			yim = new double[nX];
			for (unsigned long index = 0; index < nX; index++) {
				yim[index] = 0;
			}
		}

		item->setYim(yim);

		// Attatch the curve to the plot
		item->attach();
	}
}

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

void ot::PlotManager::calculateDataInCache(Plot1DCfg::AxisQuantity _axisQuantity) {
	for (auto itm : m_cache) {
		itm.second.second->calculateData(_axisQuantity);
	}
}
