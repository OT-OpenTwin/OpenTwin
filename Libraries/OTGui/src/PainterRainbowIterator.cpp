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

#include "OTGui/PainterRainbowIterator.h"

ot::PainterRainbowIterator::PainterRainbowIterator()
	: m_currentColourIndex(static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst))
{}

bool ot::PainterRainbowIterator::initiateWithOtherPainter(const ot::Painter2D* _painter)
{
	const ot::StyleRefPainter2D*  stylePainter = dynamic_cast<const ot::StyleRefPainter2D*>(_painter);
	bool isARainbowPainter = false;
	if (stylePainter != nullptr)
	{
		ot::ColorStyleValueEntry styleKey = stylePainter->getReferenceKey();
		if (styleKey >= ot::ColorStyleValueEntry::RainbowFirst && styleKey <= ot::ColorStyleValueEntry::RainbowLast)
		{
			m_currentColourIndex = static_cast<uint32_t>(styleKey);
			isARainbowPainter = true;
		}
	}
	return isARainbowPainter;
}


std::unique_ptr<ot::StyleRefPainter2D> ot::PainterRainbowIterator::getNextPainter()
{
	ot::ColorStyleValueEntry styleEntry = static_cast<ot::ColorStyleValueEntry>(m_currentColourIndex);

	if (m_currentColourIndex < static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowLast))
	{
		m_currentColourIndex++;
	}
	else
	{
		m_currentColourIndex = static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst); //Restart the rainbow
	}

	std::unique_ptr<ot::StyleRefPainter2D>stylePainter(new ot::StyleRefPainter2D(styleEntry));
	return stylePainter;
}
