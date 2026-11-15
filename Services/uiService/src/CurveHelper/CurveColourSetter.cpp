// @otlicense
// File: CurveColourSetter.cpp
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

#include "CurveHelper/CurveColourSetter.h"

CurveColourSetter::CurveColourSetter(const ot::Plot1DCurveCfg& _curveCfg, bool _varyLineStyles)
	: m_varyLineStyles(_varyLineStyles)
{
	m_useRainBowItterator = m_painterRainbowIterator.initiateWithOtherPainter(_curveCfg.getLinePen().getPainter());
}

CurveColourSetter::CurveColourSetter(const ot::Plot1DCurveCfg& _curveCfg, uint32_t _rainbowBufferSize, bool _varyLineStyles)
	: CurveColourSetter(_curveCfg, _varyLineStyles)
{
	m_rainbowPainterBuffer.reserve(_rainbowBufferSize);

	for (uint32_t i = 0; i < _rainbowBufferSize; i++)
	{
		m_rainbowPainterBuffer.push_back(m_painterRainbowIterator.getNextPainter().release());
	}
}

void CurveColourSetter::setPainter(ot::Plot1DCurveCfg& _curveCfg, bool _hasSingleDatapoint)
{
	ot::Painter2D* painter = m_useRainBowItterator
		? m_painterRainbowIterator.getNextPainter().release()
		: _curveCfg.getLinePen().getPainter()->createCopy();
	this->applyCurvePainter(_curveCfg, painter, _hasSingleDatapoint);
}

void CurveColourSetter::setPainter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _bufferIndex, bool _hasSingleDatapoint)
{
	ot::Painter2D* painter = m_useRainBowItterator
		? m_rainbowPainterBuffer[_bufferIndex]->createCopy()
		: _curveCfg.getLinePen().getPainter()->createCopy();
	this->applyCurvePainter(_curveCfg, painter, _hasSingleDatapoint);
}

void CurveColourSetter::applyCurvePainter(ot::Plot1DCurveCfg& _curveCfg, ot::Painter2D* _painter, bool _hasSingleDatapoint) const
{
	uint32_t rainbowIndex = 0;
	const bool hasRainbowIndex = m_varyLineStyles && m_useRainBowItterator && ot::PainterRainbowIterator::tryGetRainbowIndex(_painter, rainbowIndex);

	if (_hasSingleDatapoint)
	{
		if (hasRainbowIndex && (_curveCfg.getPointSymbol() == ot::Plot1DCurveCfg::NoSymbol || _curveCfg.getPointSymbol() == ot::Plot1DCurveCfg::Circle))
		{
			_curveCfg.setPointSymbol(ot::PainterRainbowIterator::getPointSymbolForIndex(rainbowIndex));
		}
		else if (_curveCfg.getPointSymbol() == ot::Plot1DCurveCfg::NoSymbol)
		{
			_curveCfg.setPointSymbol(ot::Plot1DCurveCfg::Circle);
		}
		_curveCfg.setLinePenStyle(ot::LineStyle::NoLine);

		_curveCfg.setPointFillPainter(_painter->createCopy());
		_curveCfg.setPointOutlinePenPainter(_painter);
	}
	else
	{
		// Only assign accessible line styles when enabled and the user has not customized the line style away from SolidLine
		if (hasRainbowIndex && _curveCfg.getLinePen().getStyle() == ot::LineStyle::SolidLine)
		{
			_curveCfg.setLinePenStyle(ot::PainterRainbowIterator::getLineStyleForIndex(rainbowIndex));
		}

		_curveCfg.setLinePenPainter(_painter);
	}
}
