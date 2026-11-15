// @otlicense
// File: PainterRainbowIterator.h
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
#include "OTGui/Painter/Painter2D.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTGui/Plot/Plot1DCurveCfg.h"
#include "OTGui/Style/PenCfg.h"

// std header
#include <cstdint>
#include <memory>

namespace ot
{
	//! @class PainterRainbowIterator
	//! @brief Iterates over the 30 theme-dependent rainbow curve painters (Rainbow0..Rainbow29).
	class OT_GUI_API_EXPORT PainterRainbowIterator
	{
	public:
		static constexpr uint32_t c_paletteSize = static_cast<uint32_t>(ColorStyleValueEntry::RainbowLast) - static_cast<uint32_t>(ColorStyleValueEntry::RainbowFirst) + 1;

		PainterRainbowIterator();
		bool initiateWithOtherPainter(const ot::Painter2D* _painter);
		std::unique_ptr<ot::Painter2D> getNextPainter();

		uint32_t getCurrentIndex() const { return m_currentColourIndex; }
		void reset() { m_currentColourIndex = 0; }

		//! @brief Extracts the rainbow palette index [0, c_paletteSize) from a StyleRefPainter2D pointing to Rainbow0..Rainbow29.
		static bool tryGetRainbowIndex(const Painter2D* _painter, uint32_t& _outIndex);

		//! @brief Returns an accessible LineStyle for the given rainbow index (0..9 Solid, 10..19 Dash, 20..29 DashDot).
		static LineStyle getLineStyleForIndex(uint32_t _index);

		//! @brief Returns a cyclic point symbol for single-point curves so curves remain distinguishable without color.
		static Plot1DCurveCfg::Symbol getPointSymbolForIndex(uint32_t _index);

	private:
		uint32_t m_currentColourIndex;
	};
}
