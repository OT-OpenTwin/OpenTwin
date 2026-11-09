// @otlicense
// File: ColorMap.h
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
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>
#include <vector>

namespace ot {

	// Serialization keys - no defines
	inline constexpr const char* c_colorMapNameKey = "Name";
	inline constexpr const char* c_colorMapPresetKey = "Preset";
	inline constexpr const char* c_colorMapInterpolationSpaceKey = "InterpolationSpace";
	inline constexpr const char* c_colorMapStopsKey = "Stops";
	inline constexpr const char* c_colorStopPositionKey = "Pos";
	inline constexpr const char* c_colorStopColorKey = "Color";

	//! @brief ColorMap preset types including modern perceptual and colorblind-safe palettes.
	enum class ColorMapPreset {
		Custom,
		Viridis,
		Plasma,
		Turbo,
		CoolWarm,
		OkabeIto,
		TolVibrant,
		Rainbow
	};

	OT_GUI_API_EXPORT std::string toString(ColorMapPreset _preset);
	OT_GUI_API_EXPORT ColorMapPreset stringToColorMapPreset(const std::string& _str);

	//! @struct ColorStop
	//! @brief A single color anchor point at a normalized position [0.0, 1.0].
	struct OT_GUI_API_EXPORT ColorStop {
		float position; //!< Normalized position [0.0f, 1.0f].
		Color color;    //!< Color at this anchor point.

		bool operator==(const ColorStop& _other) const {
			return position == _other.position && color == _other.color;
		}
		bool operator!=(const ColorStop& _other) const {
			return !(*this == _other);
		}
	};

	//! @class ColorMap
	//! @brief Represents a continuous or discrete color ramp sampled by normalized value [0.0, 1.0].
	//! Interpolation defaults to Oklab for uniform, natural transitions.
	class OT_GUI_API_EXPORT ColorMap : public ot::Serializable {
	public:
		ColorMap();
		explicit ColorMap(ColorMapPreset _preset);
		ColorMap(const std::string& _name, const std::vector<ColorStop>& _stops, ColorInterpolationSpace _space = ColorInterpolationSpace::Oklab);
		virtual ~ColorMap() = default;

		ColorMap(const ColorMap& _other) = default;
		ColorMap& operator=(const ColorMap& _other) = default;
		ColorMap(ColorMap&& _other) noexcept = default;
		ColorMap& operator=(ColorMap&& _other) noexcept = default;

		bool operator==(const ColorMap& _other) const;
		bool operator!=(const ColorMap& _other) const;

		// Base class functions
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// Stops management
		void addStop(float _position, const Color& _color);
		void clearStops();
		void setStops(const std::vector<ColorStop>& _stops);
		const std::vector<ColorStop>& getStops() const { return m_stops; }

		// Presets
		void setPreset(ColorMapPreset _preset);
		ColorMapPreset getPreset() const { return m_preset; }
		static ColorMap createPreset(ColorMapPreset _preset);

		// Sampling
		Color getColor(float _normalizedValue) const;

		// Configuration
		void setName(const std::string& _name) { m_name = _name; }
		const std::string& getName() const { return m_name; }

		void setInterpolationSpace(ColorInterpolationSpace _space) { m_interpolationSpace = _space; }
		ColorInterpolationSpace getInterpolationSpace() const { return m_interpolationSpace; }

	private:
		void sortStops();

		std::string m_name;
		ColorMapPreset m_preset;
		ColorInterpolationSpace m_interpolationSpace;
		std::vector<ColorStop> m_stops;
	};

}
