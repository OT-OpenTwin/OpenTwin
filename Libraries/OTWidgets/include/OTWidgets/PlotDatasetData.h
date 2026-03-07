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
#include "OTCore/Math.h"
#include "OTGui/Plot/Plot1DAxisCfg.h"
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_point_polar.h>

// std header
#include <vector>
#include <complex>

namespace ot {

	template<typename T> class PlotSeriesData;
	class PolarPlotDatasetData;
	class CartesianPlotDatasetData;

	//! @brief The PlotDatasetData class represents the data of a dataset that is plotted in the Plot. 
	//! It implements the QwtSeriesData interface to provide the data points for plotting.
	//! The class supports both real and complex data, and allows for different representations of complex numbers.
	//! The X and Y quantities can be f configured to represent different aspects of the data.
	class OT_WIDGETS_API_EXPORT PlotDatasetData {
		OT_DECL_NOCOPY(PlotDatasetData)
		OT_DECL_DEFMOVE(PlotDatasetData)
	public:
		//! @brief Default constructor. Creates an empty dataset with undefined quantities.
		PlotDatasetData();

		//! @brief Constructor for real data.
		//! The data can not be changed to a different quantity.
		//! @param _dataX X values of the dataset.
		//! @param _dataY Y values of the dataset.
		PlotDatasetData(const std::vector<double>& _dataX, std::vector<double>&& _dataY);

		//! @brief Constructor for real data.
		//! The data can not be changed to a different quantity.
		//! @param _dataX X values of the dataset.
		//! @param _dataY Y values of the dataset.
		PlotDatasetData(std::vector<double>&& _dataX, std::vector<double>&& _dataY);

		//! @brief Constructor for complex data.
		//! @param _dataX X values of the dataset.
		//! @param _dataY Y values of the dataset as complex numbers. The values will be converted to real / imaginary representation if the provided representation differes.
		//! @param _complexRepresentation Representation of the provided complex data. This is required to correctly convert the data to the real/imaginary representation.
		//! @param _initialXQuantity Initial quantity for the X axis.
		//! @param _initialYQuantity Initial quantity for the Y axis.
		PlotDatasetData(const std::vector<double>& _dataX, std::vector<std::complex<double>>&& _dataY, Plot1DAxisCfg::AxisQuantity _initialXQuantity = Plot1DAxisCfg::XData, Plot1DAxisCfg::AxisQuantity _initialYQuantity = Plot1DAxisCfg::Real);

		//! @brief Constructor for complex data.
		//! @param _dataX X values of the dataset.
		//! @param _dataY Y values of the dataset as complex numbers. The values will be converted to real / imaginary representation if the provided representation differes.
		//! @param _complexRepresentation Representation of the provided complex data. This is required to correctly convert the data to the real/imaginary representation.
		//! @param _initialXQuantity Initial quantity for the X axis.
		//! @param _initialYQuantity Initial quantity for the Y axis.
		PlotDatasetData(std::vector<double>&& _dataX, std::vector<std::complex<double>>&& _dataY, Plot1DAxisCfg::AxisQuantity _initialXQuantity = Plot1DAxisCfg::XData, Plot1DAxisCfg::AxisQuantity _initialYQuantity = Plot1DAxisCfg::Real);

		~PlotDatasetData();

		void updateData();

		void setXQuantity(Plot1DAxisCfg::AxisQuantity _quantity, bool _updateData = false);
		Plot1DAxisCfg::AxisQuantity getXQuantity() const { return m_xQuantity; };

		void setXQuantityScaling(const Plot1DAxisCfg::QuantityScaling& _scaling, bool _updateData = false);
		const Plot1DAxisCfg::QuantityScaling& getXQuantityScaling() const { return m_xQuantityScaling; };

		void setYQuantity(Plot1DAxisCfg::AxisQuantity _quantity, bool _updateData = false);
		Plot1DAxisCfg::AxisQuantity getYQuantity() const { return m_yQuantity; };

		void setYQuantityScaling(const Plot1DAxisCfg::QuantityScaling& _scaling, bool _updateData = false);
		const Plot1DAxisCfg::QuantityScaling& getYQuantityScaling() const { return m_yQuantityScaling; };

		CartesianPlotDatasetData* getCartesianAccessor();
		PolarPlotDatasetData* getPolarAccessor();

		size_t getSize() const;
		template <typename T> T getSample(size_t _index) const;

	private:
		friend class PlotSeriesData<QPointF>;
		friend class PlotSeriesData<QwtPointPolar>;

		double scaledValue(double _value, const Plot1DAxisCfg::QuantityScaling& _scaling) const;
		bool applyQuantityWithScaling(Plot1DAxisCfg::AxisQuantity _quantity, const Plot1DAxisCfg::QuantityScaling& _scaling, std::vector<double>& _dataTarget);

		void forgetCartesianAccessor() { m_cartesianAccessor = nullptr; };
		void forgetPolarAccessor() { m_polarAccessor = nullptr; };

		CartesianPlotDatasetData* m_cartesianAccessor;
		PolarPlotDatasetData* m_polarAccessor;

		Plot1DAxisCfg::AxisQuantity m_xQuantity;
		Plot1DAxisCfg::QuantityScaling m_xQuantityScaling;
		Plot1DAxisCfg::AxisQuantity m_yQuantity;
		Plot1DAxisCfg::QuantityScaling m_yQuantityScaling;

		std::vector<double> m_dataX;
		std::vector<std::complex<double>> m_dataY;
		bool m_canConvert;

		std::vector<double> m_calcX; //! @brief Calculated X values depending on the currently used complex representation
		std::vector<double> m_calcY; //! @brief Calculated Y values depending on the currently used complex representation
	};

}

#include "OTWidgets/PlotDatasetData.hpp"