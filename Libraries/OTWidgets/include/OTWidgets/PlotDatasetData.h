//! @file PlotDatasetData.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header

#include "OTCore/OTClassHelper.h"
#include "OTGui/Plot1DCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

#include "OTCore/ComplexNumberContainer.h"

class QwtPlotCurve;

namespace ot {

	class PolarPlotData;

	class __declspec(dllexport) PlotDatasetData
	{
		OT_DECL_NOCOPY(PlotDatasetData)
		OT_DECL_NODEFAULT(PlotDatasetData)
	public:
		//! @brief Takes over ownership over ComplexNumberContainer
		PlotDatasetData(std::vector<double>&& _dataX, ComplexNumberContainer* _complexNumberContainer);
		PlotDatasetData(std::vector<double>& _dataX, ComplexNumberContainer* _complexNumberContainer);
		~PlotDatasetData();

		PlotDatasetData(PlotDatasetData&& _other) noexcept;
		PlotDatasetData& operator = (PlotDatasetData&& _other) noexcept = default;
		
		//Depends on the ComplexNumberContainer what data is behind x and y
		const std::vector<double>& getDataX(void) const { return m_dataX; };
		const ComplexNumberContainer& getDataY(void) const { return *m_dataY; };
		
		//! @brief Takes over ownership over ComplexNumberContainer
		void overrideDataY(ComplexNumberContainer* _complexNumberContainer);
		const size_t getNumberOfDatapoints() const { return m_numberOfDatapoints; }
		
	private:
		std::vector<double> m_dataX;
		ComplexNumberContainer* m_dataY = nullptr;
		size_t m_numberOfDatapoints = 0;

		void initiate();
	};
}