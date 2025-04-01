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


		PlotDataset* newDataset = nullptr;
		//PlotDataset* newDataset = addDataset(cfg, nullptr, nullptr, 0);
		//newDataset->setStorageEntityInfo(BasicEntityInformation(curve->getCurveDataStorageId(), curve->getCurveDataStorageVersion()));
		//newDataset->updateCurveVisualization();

		// Check whether we already have the curve data


		bool loadCurveDataRequired = true;
		ot::UID dataEntityID =	curve->getCurveDataStorageId();
		ot::UID dataEntityVersion =	curve->getEntityStorageVersion();
		auto it = m_cache.find(dataEntityID);
		if (it != m_cache.end()) {
			PlotDataset* oldDataset = it->second.second;

			if (oldDataset != nullptr) {
				if (oldDataset->getStorageEntityInfo().getEntityID() == dataEntityID
					&& oldDataset->getStorageEntityInfo().getEntityVersion() == dataEntityVersion) {

					// The curve data in the previous data set is the same as the one in the new data set
					double* x = nullptr, * y = nullptr;
					long n;
					PlotDatasetData& oldDatasetData = oldDataset->getPlotData();
					PlotDatasetData newDatasetData(std::move(oldDatasetData));
					PlotDataset* newDataset = new PlotDataset(this,cfg, std::move(newDatasetData));
					loadCurveDataRequired = false;
					newDataset->attach();
					newDataset->setStorageEntityInfo(BasicEntityInformation(curve->getCurveDataStorageId(), curve->getCurveDataStorageVersion()));
					newDataset->updateCurveVisualization();
				}
			}
		}

		if (loadCurveDataRequired) {
			prefetchCurveData.push_back(std::pair<UID, UID>(curveDataStorageId, curveDataStorageVersion));
			
			newDataset->setStorageEntityInfo(BasicEntityInformation(curve->getCurveDataStorageId(), curve->getCurveDataStorageVersion()));
			newDataset->updateCurveVisualization();
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

		std::vector<double> xData;
		xData.reserve(nX);
		
		auto iX = arrayX.begin();
		for (unsigned long index = 0; index < nX; index++) {
			xData.push_back(iX->get_double());
			iX++;
		}

		std::unique_ptr<ComplexNumberContainerCartesian> yData(new ComplexNumberContainerCartesian());

		if (nYre > 0) 
		{	
			assert(nX == nYre);
			yData->m_real.reserve(nYre);
			auto iYre = arrayYre.begin();
			for (unsigned long index = 0; index < nYre; index++) {
				yData->m_real.push_back(iYre->get_double());
				iYre++;
			}
		}


		if (nYim > 0) 
		{
			assert(nX == nYim);
			yData->m_imag.reserve(nYim);
			auto iYim = arrayYim.begin();
			for (unsigned long index = 0; index < nYim; index++) {
				yData->m_imag.push_back(iYim->get_double());
				iYim++;
			}
		}

		item->setPlotData(ot::PlotDatasetData(std::move(xData), yData.release()));
		
		// Attatch the curve to the plot
		item->attach();
	}
}
