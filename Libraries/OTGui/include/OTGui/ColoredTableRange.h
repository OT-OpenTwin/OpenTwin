// @otlicense
// File: ColoredTableRange.h
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
#include "OTCore/Color.h"
#include "OTCore/Serializable.h"
#include "OTGui/TableRange.h"
#include "OTGui/ColorMap.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <memory>

namespace ot {

	// Serialization keys - no defines
	inline constexpr const char* c_coloredTableRangeRangeKey = "Range";
	inline constexpr const char* c_coloredTableRangeColorKey = "Color";
	inline constexpr const char* c_coloredTableRangeColorToKey = "ColorTo";
	inline constexpr const char* c_coloredTableRangeHasValueRangeKey = "HasValueRange";
	inline constexpr const char* c_coloredTableRangeMinValueKey = "MinValue";
	inline constexpr const char* c_coloredTableRangeMaxValueKey = "MaxValue";
	inline constexpr const char* c_coloredTableRangeHasColorMapKey = "HasColorMap";
	inline constexpr const char* c_coloredTableRangeColorMapKey = "ColorMap";

	//! @class ColoredTableRange
	//! @brief Associates an ot::TableRange with color information. Supports static colors, single-color value thresholds, 2-color gradient ranges, and multi-stop ColorMaps.
	class OT_GUI_API_EXPORT ColoredTableRange : public ot::Serializable {
	public:
		ColoredTableRange();

		// ---------------------------------------------------------------------
		// Constructors

		//! @brief Static color range: all cells in the range receive the specified color regardless of value.
		ColoredTableRange(const TableRange& _range, const Color& _color);
		//! @brief Static color range (shared pointer): all cells in the range receive the specified color regardless of value.
		ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _color);

		//! @brief Value-based threshold range (single color): only cells whose numerical value is within [_min, _max] receive _color.
		ColoredTableRange(const TableRange& _range, const Color& _color, double _min, double _max);
		//! @brief Value-based threshold range (single color, shared pointer): only cells whose numerical value is within [_min, _max] receive _color.
		ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _color, double _min, double _max);

		//! @brief Color gradient (heatmap segment): cells with values in [_min, _max] are interpolated linearly between _minColor and _maxColor.
		ColoredTableRange(const TableRange& _range, const Color& _minColor, const Color& _maxColor, double _min, double _max);
		//! @brief Color gradient (heatmap segment, shared pointer): cells with values in [_min, _max] are interpolated linearly between _minColor and _maxColor.
		ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _minColor, const Color& _maxColor, double _min, double _max);

		//! @brief ColorMap-based color range (heatmap): cells with values in [_min, _max] are interpolated using _colorMap (e.g. Viridis, Plasma).
		ColoredTableRange(const TableRange& _range, const ColorMap& _colorMap, double _min, double _max);
		//! @brief ColorMap-based color range (heatmap, shared pointer): cells with values in [_min, _max] are interpolated using _colorMap (e.g. Viridis, Plasma).
		ColoredTableRange(std::shared_ptr<TableRange> _range, const ColorMap& _colorMap, double _min, double _max);

		ColoredTableRange(const ColoredTableRange& _other);
		ColoredTableRange(ColoredTableRange&& _other) noexcept;
		virtual ~ColoredTableRange() = default;

		ColoredTableRange& operator=(const ColoredTableRange& _other);
		ColoredTableRange& operator=(ColoredTableRange&& _other) noexcept;

		bool operator==(const ColoredTableRange& _other) const;
		bool operator!=(const ColoredTableRange& _other) const { return !(*this == _other); }

		// Inherited via Serializable
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		const TableRange& getRange() const { return *m_range; }
		TableRange& getRange() { return *m_range; }
		std::shared_ptr<TableRange> getRangeShared() const { return m_range; }

		void setRange(const TableRange& _range);
		void setRange(std::shared_ptr<TableRange> _range);

		// Color access
		const Color& getColor() const { return m_color; }
		Color& getColor() { return m_color; }
		void setColor(const Color& _color) { m_color = _color; m_colorTo = _color; }

		const Color& getColorTo() const { return m_colorTo; }
		Color& getColorTo() { return m_colorTo; }
		void setColorTo(const Color& _colorTo) { m_colorTo = _colorTo; }

		const Color& getMinColor() const { return m_color; }
		const Color& getMaxColor() const { return m_colorTo; }
		void setColorGradient(const Color& _minColor, const Color& _maxColor) { m_color = _minColor; m_colorTo = _maxColor; }

		// ColorMap access
		bool hasColorMap() const { return m_hasColorMap; }
		const ColorMap& getColorMap() const { return m_colorMap; }
		ColorMap& getColorMap() { return m_colorMap; }
		void setColorMap(const ColorMap& _colorMap) { m_colorMap = _colorMap; m_hasColorMap = true; }
		void removeColorMap() { m_hasColorMap = false; }

		// Value range access
		bool hasValueRange() const { return m_hasValueRange; }
		double getMinValue() const { return m_minValue; }
		double getMaxValue() const { return m_maxValue; }
		void setValueRange(double _min, double _max) { m_minValue = _min; m_maxValue = _max; m_hasValueRange = true; }
		void removeValueRange() { m_hasValueRange = false; }

		bool isGradient() const { return m_hasValueRange && (m_hasColorMap || m_color != m_colorTo); }

		//! @brief Calculates the color for a given numerical value.
		//! Returns false if a value range is defined and _value is outside [_min, _max].
		bool calculateColor(double _value, Color& _outColor) const;

	private:
		std::shared_ptr<TableRange> m_range;
		Color m_color;
		Color m_colorTo;
		bool m_hasValueRange;
		double m_minValue;
		double m_maxValue;
		bool m_hasColorMap;
		ColorMap m_colorMap;
	};

}
