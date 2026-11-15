// @otlicense
// File: PainterRainbowIterator.cpp
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

// OpenTwin header
#include "OTGui/Painter/PainterRainbowIterator.h"

// std header
#include <array>

ot::PainterRainbowIterator::PainterRainbowIterator()
	: m_currentColourIndex(0)
{}

bool ot::PainterRainbowIterator::initiateWithOtherPainter(const ot::Painter2D* _painter)
{
	uint32_t rainbowIndex = 0;
	if (tryGetRainbowIndex(_painter, rainbowIndex))
	{
		m_currentColourIndex = rainbowIndex;
		return true;
	}

	return false;
}

std::unique_ptr<ot::Painter2D> ot::PainterRainbowIterator::getNextPainter()
{
	ot::ColorStyleValueEntry styleEntry = static_cast<ot::ColorStyleValueEntry>(
		static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst) + m_currentColourIndex
	);
	m_currentColourIndex = (m_currentColourIndex + 1) % c_paletteSize;
	return std::make_unique<ot::StyleRefPainter2D>(styleEntry);
}

bool ot::PainterRainbowIterator::tryGetRainbowIndex(const Painter2D* _painter, uint32_t& _outIndex)
{
	const ot::StyleRefPainter2D* stylePainter = dynamic_cast<const ot::StyleRefPainter2D*>(_painter);
	if (stylePainter != nullptr)
	{
		const ot::ColorStyleValueEntry styleKey = stylePainter->getReferenceKey();
		if (styleKey >= ot::ColorStyleValueEntry::RainbowFirst && styleKey <= ot::ColorStyleValueEntry::RainbowLast)
		{
			_outIndex = static_cast<uint32_t>(styleKey) - static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst);
			return true;
		}
	}

	return false;
}

ot::LineStyle ot::PainterRainbowIterator::getLineStyleForIndex(uint32_t _index)
{
	constexpr uint32_t c_blockSize = 10;
	static constexpr std::array<LineStyle, 3> c_lineStyles = {
		LineStyle::SolidLine,
		LineStyle::DashLine,
		LineStyle::DashDotLine
	};

	const uint32_t normalized = (_index % c_paletteSize) / c_blockSize;
	return c_lineStyles[normalized % c_lineStyles.size()];
}

ot::Plot1DCurveCfg::Symbol ot::PainterRainbowIterator::getPointSymbolForIndex(uint32_t _index)
{
	static constexpr std::array<Plot1DCurveCfg::Symbol, 7> c_symbols = {
		Plot1DCurveCfg::Circle,
		Plot1DCurveCfg::Square,
		Plot1DCurveCfg::Diamond,
		Plot1DCurveCfg::TriangleUp,
		Plot1DCurveCfg::TriangleDown,
		Plot1DCurveCfg::Cross,
		Plot1DCurveCfg::Star6
	};

	return c_symbols[_index % c_symbols.size()];
}
