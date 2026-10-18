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

	//! @class ColoredTableRange
	//! @brief Associates an ot::TableRange with color information. Supports static colors, single-color value thresholds, and 2-color gradient ranges.
	class OT_GUI_API_EXPORT ColoredTableRange : public ot::Serializable {
	public:
		ColoredTableRange();

		// ---------------------------------------------------------------------
		// Constructors

		//! @brief Statischer Farbbereich: Alle Zellen im Bereich erhalten unabhängig vom Wert die angegebene Farbe.
		ColoredTableRange(const TableRange& _range, const Color& _color);
		//! @brief Statischer Farbbereich (shared pointer): Alle Zellen im Bereich erhalten unabhängig vom Wert die angegebene Farbe.
		ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _color);

		//! @brief Wertbasierter Schwellwert-Bereich (einfarbig): Nur Zellen, deren numerischer Wert in [_min, _max] liegt, erhalten _color.
		ColoredTableRange(const TableRange& _range, const Color& _color, double _min, double _max);
		//! @brief Wertbasierter Schwellwert-Bereich (einfarbig, shared pointer): Nur Zellen, deren numerischer Wert in [_min, _max] liegt, erhalten _color.
		ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _color, double _min, double _max);

		//! @brief Farbverlauf / Gradient (Heatmap-Segment): Zellen mit Werten in [_min, _max] werden linear zwischen _minColor und _maxColor interpoliert.
		ColoredTableRange(const TableRange& _range, const Color& _minColor, const Color& _maxColor, double _min, double _max);
		//! @brief Farbverlauf / Gradient (Heatmap-Segment, shared pointer): Zellen mit Werten in [_min, _max] werden linear zwischen _minColor und _maxColor interpoliert.
		ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _minColor, const Color& _maxColor, double _min, double _max);

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

		// Value range access
		bool hasValueRange() const { return m_hasValueRange; }
		double getMinValue() const { return m_minValue; }
		double getMaxValue() const { return m_maxValue; }
		void setValueRange(double _min, double _max) { m_minValue = _min; m_maxValue = _max; m_hasValueRange = true; }
		void removeValueRange() { m_hasValueRange = false; }

		bool isGradient() const { return m_hasValueRange && (m_color != m_colorTo); }

		//! @brief Berechnet die Farbe für einen gegebenen numerischen Wert.
		//! Gibt false zurück, wenn ein Wertebereich definiert ist und _value außerhalb liegt.
		bool calculateColor(double _value, Color& _outColor) const;

	private:
		std::shared_ptr<TableRange> m_range;
		Color m_color;
		Color m_colorTo;
		bool m_hasValueRange;
		double m_minValue;
		double m_maxValue;
	};

}
