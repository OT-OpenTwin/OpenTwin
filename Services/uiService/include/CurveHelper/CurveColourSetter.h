// @otlicense
// File: CurveColourSetter.h
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
#include "OTGui/Plot/Plot1DCurveCfg.h"
#include "OTGui/Painter/PainterRainbowIterator.h"

class CurveColourSetter
{
public:
	CurveColourSetter(const ot::Plot1DCurveCfg& _curveCfg, bool _varyLineStyles = false);
	CurveColourSetter(const ot::Plot1DCurveCfg& _curveCfg, uint32_t _rainbowBufferSize, bool _varyLineStyles = false);
	
	void setPainter(ot::Plot1DCurveCfg& _curveCfg, bool _hasSingleDatapoint);
	void setPainter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _bufferIndex, bool _hasSingleDatapoint);

private:
	void applyCurvePainter(ot::Plot1DCurveCfg& _curveCfg, ot::Painter2D* _painter, bool _hasSingleDatapoint) const;

	ot::PainterRainbowIterator m_painterRainbowIterator;
	std::vector<ot::Painter2D*> m_rainbowPainterBuffer;
	bool m_useRainBowItterator = false;
	bool m_varyLineStyles = false;
};
