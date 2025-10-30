// @otlicense
// File: CoordinateFormatConverter.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
