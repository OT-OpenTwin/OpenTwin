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
#include "OTGui/StyleRefPainter2D.h"
#include <memory>
namespace ot
{
	class __declspec(dllexport) PainterRainbowIterator
	{
		public:
			PainterRainbowIterator();
			bool initiateWithOtherPainter(const ot::Painter2D* _painter);
			std::unique_ptr<ot::StyleRefPainter2D> getNextPainter();
	private:
		uint32_t m_currentColourIndex;
	};
}
