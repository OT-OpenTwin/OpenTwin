//! @file PlotManager.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PlotBase.h"

namespace ot {

	class __declspec(dllexport) PlotManager : public PlotBase {
	public:
		PlotManager();
		virtual ~PlotManager();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual PlotDataset* findDataset(QwtPlotCurve* _curve) override;

		virtual PlotDataset* findDataset(QwtPolarCurve* _curve) override;

		virtual std::list<PlotDataset*> findDatasets(UID _entityID) override;

		virtual std::list<PlotDataset*> getAllDatasets(void) const override;

		virtual void addDatasetToCache(PlotDataset* _dataset) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		void setFromDataBaseConfig(const Plot1DDataBaseCfg& _config);

		void removeFromCache(unsigned long long _entityID);

		bool hasCachedEntity(UID _entityID) const;

		//! \brief Updates the entity version of the specified dataset if needed.
		//! Returns false if the dataset does not exist or the version did not change.
		bool changeCachedDatasetEntityVersion(UID _entityID, UID _newEntityVersion);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected virtual methods

	protected:
		virtual void clearCache(void) override;
		virtual void detachAllCached(void) override;
	
		// ###########################################################################################################################################################################################################################################################################################################################

		// Private

	private:
		std::map<UID, std::pair<UID, PlotDataset*>> m_cache;

		void importData(const std::string& _projectName, const std::list<Plot1DCurveCfg>& _curvesToImport);
	};

}