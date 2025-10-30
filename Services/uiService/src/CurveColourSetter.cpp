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

#include "CurveColourSetter.h"

CurveColourSetter::CurveColourSetter(ot::Plot1DCurveCfg& _curveCfg)
{
	m_useRainBowItterator = m_painterRainbowIterator.initiateWithOtherPainter(_curveCfg.getLinePen().getPainter());
	m_painterRainbowIterator.initiateWithOtherPainter(_curveCfg.getLinePen().getPainter());
}

CurveColourSetter::CurveColourSetter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _rainbowBufferSize)
	:CurveColourSetter(_curveCfg)
{
	m_rainbowPainterBuffer.reserve(_rainbowBufferSize);

	for (int i = 0; i < _rainbowBufferSize; i++)
	{
		m_rainbowPainterBuffer.push_back(m_painterRainbowIterator.getNextPainter().release());
	}
}

void CurveColourSetter::setPainter(ot::Plot1DCurveCfg& _curveCfg, bool _hasSingleDatapoint)
{
	if (_hasSingleDatapoint)
	{
		_curveCfg.setPointSymbol(ot::Plot1DCurveCfg::Circle);
		_curveCfg.setLinePenStyle(ot::LineStyle::NoLine);

		ot::Painter2D* painter = nullptr;
		if (m_useRainBowItterator)
		{
			painter = m_painterRainbowIterator.getNextPainter().release();
		}
		else
		{
			painter = _curveCfg.getLinePen().getPainter()->createCopy();
		}
		_curveCfg.setPointFillPainter(painter->createCopy());
		_curveCfg.setPointOutlinePenPainter(painter);
	}
	else
	{
		ot::Painter2D* painter = nullptr;
		if (m_useRainBowItterator)
		{
			painter = m_painterRainbowIterator.getNextPainter().release();
		}
		else
		{
			painter = _curveCfg.getLinePen().getPainter()->createCopy();
		}

		_curveCfg.setLinePenPainter(painter);
	}
}

void CurveColourSetter::setPainter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _bufferIndex, bool _hasSingleDatapoint)
{
	if (_hasSingleDatapoint)
	{
		_curveCfg.setPointSymbol(ot::Plot1DCurveCfg::Circle);
		_curveCfg.setLinePenStyle(ot::LineStyle::NoLine);

		ot::Painter2D* painter = nullptr;
		if (m_useRainBowItterator)
		{
			painter = m_rainbowPainterBuffer[_bufferIndex]->createCopy();
		}
		else
		{
			painter = _curveCfg.getLinePen().getPainter()->createCopy();
		}
		_curveCfg.setPointFillPainter(painter->createCopy());
		_curveCfg.setPointOutlinePenPainter(painter);

	}
	else
	{
		ot::Painter2D* painter = nullptr;
		if (m_useRainBowItterator)
		{
			painter = m_rainbowPainterBuffer[_bufferIndex]->createCopy();
		}
		else
		{
			painter = _curveCfg.getLinePen().getPainter()->createCopy();
		}

		_curveCfg.setLinePenPainter(painter);
	}
}
