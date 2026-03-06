// @otlicense
// File: Plot1DAxisCfg.h
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
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning (disable:4251)

namespace ot {

	class Plot1DCfg;

	class OT_GUI_API_EXPORT Plot1DAxisCfg : public Serializable {
		OT_DECL_DEFCOPY(Plot1DAxisCfg)
		OT_DECL_DEFMOVE(Plot1DAxisCfg)
	public:
		enum AxisQuantity : uint8_t {
			Undefined,
			XData,
			Magnitude,
			Phase,
			Real,
			Imaginary
		};

		enum AxisScalingFlag : uint32_t
		{
			NoScaling   = 0 << 0, //! @brief No scaling applied to the axis (Linear scale).
			Autoscale   = 1 << 0, //! @brief Automatic scaling applied to the axis based on the data range.
			Logarithmic = 1 << 1, //! @brief Logarithmic scaling applied to the axis.
		};
		typedef Flags<AxisScalingFlag, uint32_t> AxisScaling;

		enum QuantityScalingFlag : uint32_t
		{
			NoQuantityScaling = 0 << 0, //! @brief No scaling applied to the quantities. f(x)=x.
			DB10              = 1 << 1, //! @brief Decibel (dB) scaling applied to the quantities. f(x)=(10 * log10(x)).
			DB20              = 1 << 2  //! @brief Decibel (dB) scaling applied to the quantities. f(x)=(20 * log10(x)).
		};
		typedef Flags<QuantityScalingFlag, uint32_t> QuantityScaling;

		static std::string toString(AxisQuantity _quantity);
		static AxisQuantity stringToAxisQuantity(const std::string& _quantity);

		static std::string toString(QuantityScalingFlag _scaling);
		static QuantityScalingFlag stringToQuantityScalingFlag(const std::string& _scaling);

		static std::list<std::string> getAxisQuantityStringList();

		static std::list<std::string> getQuantityScalingStringList();

		Plot1DAxisCfg();
		virtual ~Plot1DAxisCfg();

		bool operator==(const Plot1DAxisCfg& _other) const;
		bool operator!=(const Plot1DAxisCfg& _other) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setMin(double _min) { m_min = _min; };
		double getMin() const { return m_min; };

		void setMax(double _max) { m_max = _max; };
		double getMax() const { return m_max; };

		void setQuantity(AxisQuantity _quantity) { m_axisQuantity = _quantity; };
		AxisQuantity getQuantity() const { return m_axisQuantity; };

		inline void setScaling(const AxisScaling& _scaling) { m_axisScaling = _scaling; };
		constexpr const AxisScaling& getScaling() const { return m_axisScaling; };

		//! @brief Set whether automatic scaling is enabled for the axis.
		//! When enabled, the axis range will be automatically determined based on the data being plotted.
		//! When disabled, the axis range will be determined by the manually set minimum and maximum values.
		//! @param _autoScaleEnabled True to enable automatic scaling, false to disable it.
		inline void setIsAutoScale(bool _autoScaleEnabled) noexcept { m_axisScaling.set(AxisScalingFlag::Autoscale, _autoScaleEnabled); };

		//! @brief Get whether automatic scaling is enabled for the axis.
		//! @ref setIsAutoScale
		constexpr bool getIsAutoScale() const noexcept { return m_axisScaling.has(AxisScalingFlag::Autoscale); };

		//! @brief @brief Set whether logarithmic scaling is enabled for the axis.
		//! When enabled, the axis will use a logarithmic scale.
		//! When disabled, the axis will use a linear scale.
		//! @param _logScaleEnabled True to enable logarithmic scaling, false to disable it.
		inline void setIsLogScale(bool _logScaleEnabled) noexcept { m_axisScaling.set(AxisScalingFlag::Logarithmic, _logScaleEnabled); };

		//! @brief Get whether logarithmic scaling is enabled for the axis.
		//! @ref setIsLogScale
		constexpr bool getIsLogScale() const noexcept { return m_axisScaling.has(AxisScalingFlag::Logarithmic); };

		//! @brief Set the quantity scaling flags for the axis.
		//! @param _scaling The quantity scaling flags to set for the axis.
		inline void setQuantityScaling(const QuantityScaling& _scaling) { m_quantityScaling = _scaling; };
		
		//! @brief Get the quantity scaling flags for the axis.
		constexpr const QuantityScaling& getQuantityScaling() const { return m_quantityScaling; };

		//! @brief Set whether decibel (dB) scaling is enabled for the axis using 10 * log_10.
		//! @note The plot allows for both dB10 and dB20 scaling to be enabled at the same time.
		//! If this behavior is not desired if must be controlled externally (e.g. property not allowing both to be enabled at the same time).
		//! @param _db10ScaleEnabled True to enable dB10 scaling, false to disable it.
		inline void setIsQuantityDB10Scale(bool _db10ScaleEnabled) noexcept { m_quantityScaling.set(QuantityScalingFlag::DB10, _db10ScaleEnabled); };

		//! @brief Get whether decibel (dB) scaling is enabled for the axis using 10 * log_10.
		constexpr bool getIsQuantityDB10Scale() const noexcept { return m_quantityScaling.has(QuantityScalingFlag::DB10); };

		//! @brief Set whether decibel (dB) scaling is enabled for the axis using 20 * log_10.
		//! @note The plot allows for both dB10 and dB20 scaling to be enabled at the same time.
		//! If this behavior is not desired if must be controlled externally (e.g. property not allowing both to be enabled at the same time).
		//! @param _db20ScaleEnabled True to enable dB20 scaling, false to disable it.
		inline void setIsQuantityDB20Scale(bool _db20ScaleEnabled) noexcept { m_quantityScaling.set(QuantityScalingFlag::DB20, _db20ScaleEnabled); };

		//! @brief Get whether decibel (dB) scaling is enabled for the axis using 20 * log_10.
		constexpr bool getIsQuantityDB20Scale() const noexcept { return m_quantityScaling.has(QuantityScalingFlag::DB20); };

		inline void setAutoDetermineAxisLabel(bool _autoDetermineAxisLabel) { m_autoDetermineAxisLabel = _autoDetermineAxisLabel; };
		constexpr bool getAutoDetermineAxisLabel() const { return m_autoDetermineAxisLabel; };

		void setAxisLabel(const std::string& _label) { m_axisLabel = _label; };
		const std::string& getAxisLabel() const { return m_axisLabel; };

		//! @brief Returns the axis label to be displayed.
		//! Will include the axis label, quantity name and quantity scaling information.
		std::string getDisplayAxisLabel(const Plot1DCfg& _plotCfg) const;

	private:
		double m_min;
		double m_max;

		bool m_autoDetermineAxisLabel = true;
		std::string m_axisLabel = "";

		std::string getXDataLabel(const Plot1DCfg& _plotCfg) const;
		std::string getYDataLabel(const Plot1DCfg& _plotCfg) const;

		AxisQuantity m_axisQuantity = AxisQuantity::Undefined;
		AxisScaling m_axisScaling = AxisScalingFlag::Autoscale;
		QuantityScaling m_quantityScaling = QuantityScalingFlag::NoQuantityScaling;

	};

}