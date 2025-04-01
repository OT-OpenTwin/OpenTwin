#pragma once
#include "PlotDatasetData.h"
namespace ot
{
	struct PointsContainer
	{
		const std::vector<double>* m_xData;
		const std::vector<double>* m_yData;
	};
	
	class CoordinateTransformer
	{
	public:
		PointsContainer defineXYPoints(const PlotDatasetData& _datasetData, Plot1DCfg::AxisQuantity _axisQuantity);
		~CoordinateTransformer();
	private:
		ComplexNumberContainer* m_buffer = nullptr;

		
		std::vector<double> calculateMagnitude(const PlotDatasetData& _datasetData);
		//! @brief Phase in degree
		std::vector<double> calculatePhase(const PlotDatasetData& _datasetData);
		std::vector<double> calculateImag(const PlotDatasetData& _datasetData);
		std::vector<double> calculateReal(const PlotDatasetData& _datasetData);


		std::vector<double>* getBufferedPhase(const PlotDatasetData& _datasetData, bool allZeros);
		std::vector<double>* getBufferedMagnitude(const PlotDatasetData& _datasetData, bool allZeros);
		std::vector<double>* getBufferedImaginary(const PlotDatasetData& _datasetData, bool allZeros);
		std::vector<double>* getBufferedReal(const PlotDatasetData& _datasetData, bool allZeros);
	};
}
