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
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGui/PainterRainbowIterator.h"
class CurveColourSetter
{
public:
	CurveColourSetter(ot::Plot1DCurveCfg& _curveCfg);
	CurveColourSetter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _rainbowBufferSize);
	
	void setPainter(ot::Plot1DCurveCfg& _curveCfg, bool _hasSingleDatapoint);
	void setPainter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _bufferIndex ,bool _hasSingleDatapoint);

private:
	ot::PainterRainbowIterator m_painterRainbowIterator;
	std::vector<ot::StyleRefPainter2D*> m_rainbowPainterBuffer;
	bool m_useRainBowItterator = false;
};
