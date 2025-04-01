#pragma once
#include "PlotDatasetData.h"
namespace ot
{
	struct __declspec(dllexport) PointsContainer
	{
		const std::vector<double>* m_xData;
		const std::vector<double>* m_yData;
	};
	
	class __declspec(dllexport) CoordinateFormatConverter
	{
	public:
		PointsContainer defineXYPoints(const PlotDatasetData& _datasetData, Plot1DCfg::AxisQuantity _axisQuantity);
		~CoordinateFormatConverter();
	private:
		ComplexNumberContainer* m_buffer = nullptr;

		
		std::vector<double> calculateMagnitude(const PlotDatasetData& _datasetData);
		//! @brief Phase in degree
		std::vector<double> calculatePhase(const PlotDatasetData& _datasetData);
		std::vector<double> calculateImag(const PlotDatasetData& _datasetData);
		std::vector<double> calculateReal(const PlotDatasetData& _datasetData);


		std::vector<double>* getBufferedPhase(const PlotDatasetData& _datasetData, bool _allZeros, bool _keepBuffer = false);
		std::vector<double>* getBufferedMagnitude(const PlotDatasetData& _datasetData, bool _allZeros, bool _keepBuffer = false);
		std::vector<double>* getBufferedImaginary(const PlotDatasetData& _datasetData, bool _allZeros, bool _keepBuffer = false);
		std::vector<double>* getBufferedReal(const PlotDatasetData& _datasetData, bool _allZeros, bool _keepBuffer = false);

		void resetBufferPolar(bool _keepBuffer);
		void resetBufferCartesian(bool _keepBuffer);
	};
}
