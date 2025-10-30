// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/PlotBase.h"

namespace ot {

	//! @class Plot
	//! @brief Basic plot.
	//! The plot allows multiple Datasets to be added for the same UID.
	class OT_WIDGETS_API_EXPORT Plot : public PlotBase {
		OT_DECL_NOCOPY(Plot)
		OT_DECL_NOMOVE(Plot)
	public:
		Plot();
		virtual ~Plot();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual PlotDataset* findDataset(QwtPlotCurve* _curve) override;

		virtual PlotDataset* findDataset(QwtPolarCurve* _curve) override;

		virtual std::list<PlotDataset*> findDatasets(UID _entityID) override;

		virtual std::list<PlotDataset*> getAllDatasets(void) const override;

		virtual void addDatasetToCache(PlotDataset* _dataset) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		//! @brief Removes all datasets assigned to the provided UID.
		//! @param _entityID Entity UID.
		void removeFromCache(UID _entityID);

		//! @brief Returns true if at least one dataset exists for the specified UID.
		//! @param _entityID Entity UID.
		bool hasCachedEntity(UID _entityID) const;

		//! @brief Updates the entity version of the specified dataset if needed.
		//! Returns false if the dataset does not exist or the version did not change.
		bool changeCachedDatasetEntityVersion(UID _entityID, UID _newEntityVersion);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected virtual methods

	protected:
		virtual void clearCache(void) override;
		virtual void detachAllCached(void) override;
		
	private:
		std::map<UID, std::list<PlotDataset*>> m_cache;
		
	};

}