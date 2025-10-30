// @otlicense
// File: PlotDatasetData.h
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

// OpenTwin header

#include "OTCore/ComplexNumberContainer.h"
#include "OTGui/Plot1DCfg.h"
#include "OTWidgets/WidgetTypes.h"

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