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


void ot::PlotManager::importData(const std::string& _projectName, const std::list<Plot1DCurveCfg>& _entitiesToImport) {
	if (_entitiesToImport.empty()) return;

	std::list<std::pair<UID, UID>> prefetchCurves;
	for (const Plot1DCurveCfg& item : _entitiesToImport) {
		prefetchCurves.push_back(std::pair<UID, UID>(item.getId(), item.getVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchCurves);

	std::list<std::pair<UID, UID>> prefetchCurveData;
	std::list< PlotDataset*> curves;

	for (auto item : _entitiesToImport) {
		UID entityID = item.getId();
		UID entityVersion = item.getVersion();

		EntityHandler handler;
		EntityBase* baseEntity = handler.readEntityFromEntityIDandVersion(entityID, entityVersion);
		std::unique_ptr<EntityResult1DCurve> curve(dynamic_cast<EntityResult1DCurve*>(baseEntity));

		// Read the curve data item
		UID curveDataStorageId = curve->getCurveDataStorageId();
		UID curveDataStorageVersion = curve->getCurveDataStorageVersion();
		baseEntity = handler.readEntityFromEntityIDandVersion(curveDataStorageId, curveDataStorageVersion);
		std::unique_ptr<EntityResult1DCurveData> curveData(dynamic_cast<EntityResult1DCurveData*>(baseEntity));
		std::string curveLabel = item.getName();

		ot::Color colour = curve->getColor();

		std::string xAxisLabel = curve->getAxisLabelX();
		std::string yAxisLabel = curve->getAxisLabelY();

		std::string xAxisUnit = curve->getUnitX();
		std::string yAxisUnit = curve->getUnitY();

		std::string axisTitleX = xAxisLabel + " [" + xAxisUnit + "]";
		std::string axisTitleY = yAxisLabel + " [" + yAxisUnit + "]";

		PlotDataset* newDataset = addDataset(curveLabel.c_str(), nullptr, nullptr, 0);

		newDataset->setEntityID(entityID);
		newDataset->setEntityVersion(entityVersion);
		newDataset->setCurveEntityID(curveDataStorageId);
		newDataset->setCurveEntityVersion(curveDataStorageVersion);
		newDataset->setTreeItemID(item.getTreeId());
		newDataset->setDimmed(item.getDimmed(), false);

		newDataset->setCurvePointsVisible(false, false);

		newDataset->setCurveColor(QColor(colour.r(), colour.g(), colour.b()), false);
		newDataset->setCurveTitle(curveLabel.c_str());
		newDataset->setAxisTitleX(axisTitleX);
		newDataset->setAxisTitleY(axisTitleY);

		newDataset->updateVisualization();

		// Check whether we already have the curve data

		bool loadCurveDataRequired = true;

		std::map<UID, std::pair<UID, PlotDataset*>>& cache = this->getCache();
		auto it = cache.find(newDataset->getEntityID());
		if (it != cache.end()) {
			PlotDataset* oldDataset = it->second.second;

			if (oldDataset != nullptr) {
				if (oldDataset->getCurveEntityID() == newDataset->getCurveEntityID()
					&& oldDataset->getCurveEntityVersion() == newDataset->getCurveEntityVersion()) {
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

		cache.insert_or_assign(newDataset->getEntityID(), std::pair<UID, PlotDataset*>(newDataset->getEntityVersion(), newDataset));
	}

	if (prefetchCurveData.empty()) return; // Nothing to load

	// Now load the curve data
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchCurveData);

	for (PlotDataset* item : curves) {
		// Here we get the storage data of the underlying curve data
		UID entityID = item->getCurveEntityID();
		UID entityVersion = item->getCurveEntityVersion();

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
